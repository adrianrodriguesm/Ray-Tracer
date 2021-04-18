#pragma once
#include "Scene/Scene.h"


namespace rayTracer
{
	class Grid
	{
	public:
		Grid();
		Grid(const std::vector<Object*> objects);
		virtual ~Grid();
		
		// Set up grid cells
		void BuildGrid();   
		RayCastHit Intercepts(Ray& r); 
		bool InterceptsShadows(Ray& r, float lightDist);
	private:
		Vec3 FindMinBounds();
		Vec3 FindMaxBounds();
		RayCastHit GetClossestHitInsideCell(std::vector<Object*> sceneObjects, Ray& ray);
		RayCastHit GetShadowHitInsideCell(std::vector<Object*> sceneObjects, Ray& ray, float lightDist);
	private:
		std::vector<Object*> m_SceneObjects;
		std::vector<std::vector<Object*>> m_Cells;
		std::unordered_map<Object*, RayCastHit> m_CachedInterceptions;
		// Number of cells in the x, y, and z directions
		Vec3Int m_CellNumberPerDim;
		// factor that allows to vary the number of cells
		float m_CellMultiplierFactor = 2.0f; // Approximately 8 times more cells than objects

		AABB m_BBox;	
	};
}
