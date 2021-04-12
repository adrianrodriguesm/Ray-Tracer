#include "Grid.h"
#include "Core/Base.h"
#include "Core/Constant.h"
namespace rayTracer
{
	Grid::Grid()
	{
	}
	Grid::Grid(const std::vector<Object*> objects)
		: m_SceneObjects(objects)
	{
	}
	Grid::~Grid()
	{
	}
	void Grid::BuildGrid()
	{
		m_BBox.Min = FindMinBounds();
		m_BBox.Max = FindMaxBounds();

		// Compute number of cells in xyz directions
		uint32_t numObjects = (uint32_t)m_SceneObjects.size();
		// Cell dimentions
		Vec3 dimensions = m_BBox.Max - m_BBox.Min;
		float s = powf((dimensions.x * dimensions.y * dimensions.z) / numObjects, 1.f/3.f);

		for (uint32_t index = 0; index < 3; index++)
		{
			m_CellNumberPerDim[index] = (m_CellMultiplierFactor * dimensions[index] / s) + 1;
		}
		uint32_t numCells = m_CellNumberPerDim.x * m_CellNumberPerDim.y * m_CellNumberPerDim.z;
		//m_Cells.reserve(numCells);
		m_Cells.resize(numCells);
		for (uint32_t index = 0; index < numObjects; index++)
		{
			AABB objectAABB = m_SceneObjects[index]->GetBoundingBox();
			// Compute the cell indices for the corners of the bounding box of the obj
			Vec3Int min, max;
			for (uint32_t i = 0; i < 3; i++)
			{
				int cellNumberDim = m_CellNumberPerDim[i];
				min[i] = std::clamp((int)((objectAABB.Min[i] - m_BBox.Min[i]) * cellNumberDim / (m_BBox.Max[i] - m_BBox.Min[i])),
									0, cellNumberDim - 1);
				max[i] = std::clamp(int((objectAABB.Max[i] - m_BBox.Min[i]) * cellNumberDim / (m_BBox.Max[i] - m_BBox.Min[i])),
									0, cellNumberDim - 1);
			}
			// Add scene object to the overlaped cells
			for (int iz = min.z; iz <= max.z; iz++)
			{
				for (int iy = min.y; iy <= max.y; iy++)
				{
					for (int ix = min.x; ix <= max.x; ix++)
					{
						int cellIndex = ix + m_CellNumberPerDim.x * iy + m_CellNumberPerDim.x * m_CellNumberPerDim.y * iz;
						m_Cells[cellIndex].push_back(m_SceneObjects[index]);
					}
				}
			}


		}
		

	}
	RayCastHit Grid::Intercepts(Ray& ray)
	{	
		Vec3 min, max;
		Vec3 delta;
		for (uint32_t index = 0; index < 3; index++)
		{
			delta[index] = 1 / ray.Direction[index];
			if (delta[index] >= 0)
			{
				min[index] = (m_BBox.Min[index] - ray.Origin[index]) * delta[index];
				max[index] = (m_BBox.Max[index] - ray.Origin[index]) * delta[index];
			}
			else
			{
				min[index] = (m_BBox.Max[index] - ray.Origin[index]) * delta[index];
				max[index] = (m_BBox.Min[index] - ray.Origin[index]) * delta[index];
			}
		}
		// Largest entry point
		float tMin = std::max({ min.x, min.y, min.z });
		// Nearest exit point
		float tMax = std::min({ max.x, max.y, max.z });

		if (tMin > tMax)
			return { false };

		
		Vec3Int cellCoordinates;
		// Check if ray start inside the grid
		if (m_BBox.IsInside(ray.Origin))
		{
			for (uint32_t index = 0; index < 3; index++)
			{
				cellCoordinates[index] = std::clamp((int)((ray.Origin[index] - m_BBox.Min[index]) * m_CellNumberPerDim[index] / (m_BBox.Max[index] - m_BBox.Min[index])),
												0, m_CellNumberPerDim[index] - 1);
			}
		}
		else
		{
			// Initial hit point with grid's BB
			Vec3 point = ray.Origin + (ray.Direction * tMin);
			for (uint32_t index = 0; index < 3; index++)
			{
				cellCoordinates[index] = std::clamp((int)((point[index] - m_BBox.Min[index]) * m_CellNumberPerDim[index] / (m_BBox.Max[index] - m_BBox.Min[index])),
					0, m_CellNumberPerDim[index] - 1);
			}
		}

		// Find the nearest intersection
		Vec3 deltaT = (max - min) / m_CellNumberPerDim;
		Vec3 next; 
		Vec3Int step, stop;
		for (uint32_t index = 0; index < 3; index++)
		{
			if (delta[index] > 0)
			{
				next[index] = min[index] + (cellCoordinates[index] + 1) * deltaT[index];
				step[index] ++;
				stop[index] = m_CellNumberPerDim[index];
			}
			else
			{
				if (delta[index] == 0)
					next[index] = FLOAT_MAX;
				else
					next[index] = min[index] + (m_CellNumberPerDim[index] - cellCoordinates[index]) * deltaT[index];
				
				step[index] --;
				stop[index] --;
			}
		}
		m_CachedInterceptions.clear();
		// Grid traversal loop
		while (true)
		{
			// Get the objects of that cell
			uint32_t index = cellCoordinates.x + m_CellNumberPerDim.x * cellCoordinates.y + m_CellNumberPerDim.x * m_CellNumberPerDim.y * cellCoordinates.z;
			auto& sceneObject = m_Cells[index];

			if (next.x < next.y && next.x < next.z)
			{			
				RayCastHit hit = GetClossestHitInsideCell(sceneObject, ray);
				if (hit && hit.Tdist < next.x)
					return hit;

				next.x += deltaT.x;
				cellCoordinates.x += step.x;

				if (cellCoordinates.x == stop.x)
					return { false };
			}
			else if (next.y < next.z)
			{

				RayCastHit hit = GetClossestHitInsideCell(sceneObject, ray);
				if (hit && hit.Tdist < next.y)
					return hit;

				next.y += deltaT.y;
				cellCoordinates.y += step.y;

				if (cellCoordinates.y == stop.y)
					return { false };
			}
			else
			{
				RayCastHit hit = GetClossestHitInsideCell(sceneObject, ray);
				if (hit && hit.Tdist < next.z)
					return hit;

				next.z += deltaT.z;
				cellCoordinates.z += step.z;

				if (cellCoordinates.z == stop.z)
					return { false };
			}
			

		}
		return {false};
	}
	bool Grid::InterceptsShadows(Ray& ray)
	{
		Vec3 min, max;
		Vec3 delta;
		for (uint32_t index = 0; index < 3; index++)
		{
			delta[index] = 1 / ray.Direction[index];
			if (delta[index] >= 0)
			{
				min[index] = (m_BBox.Min[index] - ray.Origin[index]) * delta[index];
				max[index] = (m_BBox.Max[index] - ray.Origin[index]) * delta[index];
			}
			else
			{
				min[index] = (m_BBox.Max[index] - ray.Origin[index]) * delta[index];
				max[index] = (m_BBox.Min[index] - ray.Origin[index]) * delta[index];
			}
		}
		// Largest entry point
		float tMin = std::max({ min.x, min.y, min.z });
		// Nearest exit point
		float tMax = std::min({ max.x, max.y, max.z });

		if (tMin > tMax)
			return { false };


		Vec3Int cellCoordinates;
		// Check if ray start inside the grid
		if (m_BBox.IsInside(ray.Origin))
		{
			for (uint32_t index = 0; index < 3; index++)
			{
				cellCoordinates[index] = std::clamp((int)((ray.Origin[index] - m_BBox.Min[index]) * m_CellNumberPerDim[index] / (m_BBox.Max[index] - m_BBox.Min[index])),
					0, m_CellNumberPerDim[index] - 1);
			}
		}
		else
		{
			// Initial hit point with grid's BB
			Vec3 point = ray.Origin + (ray.Direction * tMin);
			for (uint32_t index = 0; index < 3; index++)
			{
				cellCoordinates[index] = std::clamp((int)((point[index] - m_BBox.Min[index]) * m_CellNumberPerDim[index] / (m_BBox.Max[index] - m_BBox.Min[index])),
					0, m_CellNumberPerDim[index] - 1);
			}
		}

		// Find the nearest intersection
		Vec3 deltaT = (max - min) / m_CellNumberPerDim;
		Vec3 next;
		Vec3Int step, stop;
		for (uint32_t index = 0; index < 3; index++)
		{
			if (delta[index] > 0)
			{
				next[index] = min[index] + (cellCoordinates[index] + 1) * deltaT[index];
				step[index] ++;
				stop[index] = m_CellNumberPerDim[index];
			}
			else
			{
				if (delta[index] == 0)
					next[index] = FLOAT_MAX;
				else
					next[index] = min[index] + (m_CellNumberPerDim[index] - cellCoordinates[index]) * deltaT[index];

				step[index] --;
				stop[index] --;
			}
		}
		m_CachedInterceptions.clear();
		// Grid traversal loop
		while (true)
		{
			// Get the objects of that cell
			uint32_t index = cellCoordinates.x + m_CellNumberPerDim.x * cellCoordinates.y + m_CellNumberPerDim.x * m_CellNumberPerDim.y * cellCoordinates.z;
			auto& sceneObject = m_Cells[index];

			if (next.x < next.y && next.x < next.z)
			{
				RayCastHit hit = GetClossestHitInsideCell(sceneObject, ray);
				if (hit)
					return hit;

				next.x += deltaT.x;
				cellCoordinates.x += step.x;

				if (cellCoordinates.x == stop.x)
					return { false };
			}
			else if (next.y < next.z)
			{

				RayCastHit hit = GetClossestHitInsideCell(sceneObject, ray);
				if (hit)
					return hit;

				next.y += deltaT.y;
				cellCoordinates.y += step.y;

				if (cellCoordinates.y == stop.y)
					return { false };
			}
			else
			{
				RayCastHit hit = GetClossestHitInsideCell(sceneObject, ray);
				if (hit)
					return hit;

				next.z += deltaT.z;
				cellCoordinates.z += step.z;

				if (cellCoordinates.z == stop.z)
					return { false };
			}


		}
		return { false };
	}
	Vec3 Grid::FindMinBounds()
	{
		Vec3 minBounds = { FLOAT_MAX };
		AABB bbox;
		for (auto& SceneObject : m_SceneObjects)
		{
			bbox = SceneObject->GetBoundingBox();
			minBounds.x = bbox.Min.x < minBounds.x ? bbox.Min.x : minBounds.x;
			minBounds.y = bbox.Min.y < minBounds.y ? bbox.Min.y : minBounds.y;
			minBounds.z = bbox.Min.z < minBounds.z ? bbox.Min.z : minBounds.z;
		}

		for (auto& component : minBounds.components)
		{
			component -= EPSILON;
		}

		return minBounds;
	}
	Vec3 Grid::FindMaxBounds()
	{
		Vec3 maxBounds = { FLOAT_MIN };
		AABB bbox;
		for (auto& SceneObject : m_SceneObjects)
		{
			bbox = SceneObject->GetBoundingBox();
			maxBounds.x = bbox.Max.x > maxBounds.x ? bbox.Max.x : maxBounds.x;
			maxBounds.y = bbox.Max.y > maxBounds.y ? bbox.Max.y : maxBounds.y;
			maxBounds.z = bbox.Max.z > maxBounds.z ? bbox.Max.z : maxBounds.z;
		}

		for (auto& component : maxBounds.components)
		{
			component += EPSILON;
		}

		return maxBounds;
	}
	RayCastHit Grid::GetClossestHitInsideCell(std::vector<Object*> sceneObjects, Ray& ray)
	{
		if (sceneObjects.empty())
			return { false };

		RayCastHit hit;
		float tDist = FLOAT_MAX;
		for (auto& object : sceneObjects)
		{
			// Mailbox: cached the objects in order to avoid the calculation 
			// of the interception  of an object more than one
			RayCastHit hitTemp;
			if (m_CachedInterceptions.find(object) == m_CachedInterceptions.end())
			{
				hitTemp = object->Intercepts(ray);
				m_CachedInterceptions[object] = hitTemp;
			}
			else
				hitTemp = m_CachedInterceptions[object];

			if (hitTemp && hitTemp.Tdist < tDist)
			{
				tDist = hitTemp.Tdist;
				hit = hitTemp;
			}
		}
		return hit;
	}
}