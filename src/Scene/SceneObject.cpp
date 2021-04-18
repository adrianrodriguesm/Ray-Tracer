#include "pch.h"
#include "SceneObject.h"
#include "Core/Constant.h"
#include "Math/PerlinNoise.hpp"
namespace rayTracer
{
	Object::~Object()
	{
		//delete m_Material;
	}
	Triangle::Triangle(Vec3& P0, Vec3& P1, Vec3& P2)
	{
		m_Points[0] = P0; m_Points[1] = P1; m_Points[2] = P2;

		//Calculate the Min and Max for bounding box
		//Vector min = Vector(+FLT_MAX, +FLT_MAX, +FLT_MAX);
		//Vector max = Vector(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		// enlarge the bounding box a bit just in case...
		//min -= EPSILON;
		//min += EPSILON;

		// Calculate Normal
		Vec3 Edge1 = m_Points[1] - m_Points[0];
		Vec3 Edge2 = m_Points[2] - m_Points[0];
		m_Normal = CrossProduct(Edge1, Edge2);
		m_Normal.Normalized();

		
		CalculateAABB();
	}

	void Triangle::CalculateAABB()
	{
		float minX = std::min({ m_Points[0].x, m_Points[1].x, m_Points[2].x });
		float minY = std::min({ m_Points[0].y, m_Points[1].y, m_Points[2].y });
		float minZ = std::min({ m_Points[0].z, m_Points[1].z, m_Points[2].z });
		Vec3 min = { minX, minY, minZ };

		float maxX = std::max({ m_Points[0].x, m_Points[1].x, m_Points[2].x });
		float maxY = std::max({ m_Points[0].y, m_Points[1].y, m_Points[2].y });
		float maxZ = std::max({ m_Points[0].z, m_Points[1].z, m_Points[2].z });
		Vec3 max = { maxX, maxY, maxZ };

		m_BoundingBox = { min, max };
	}

	//
	// Ray/Triangle intersection test using Tomas Moller-Ben Trumbore algorithm.
	RayCastHit Triangle::Intercepts(Ray& ray)
	{
		Vec3 Edge1 = m_Points[1] - m_Points[0];
		Vec3 Edge2 = m_Points[2] - m_Points[0];
		//Vec3 N = CrossProduct(Edge1, Edge2);
		float det = -DotProduct(ray.Direction, m_Normal);
		float invdet = 1.0f / det;
		Vec3 AO = ray.Origin - m_Points[0];
		Vec3 DAO = CrossProduct(AO, ray.Direction);
		float u = DotProduct(Edge2, DAO) * invdet;
		float v = -DotProduct(Edge1, DAO) * invdet;

		RayCastHit hit;
		hit.Tdist = DotProduct(AO, m_Normal) * invdet;
		hit.Hit = (det >= 1e-6 && hit.Tdist >= 0.0 && u >= 0.0 && v >= 0.0 && (u + v) <= 1.0f);
		hit.Object = this;
		hit.InterceptionPoint = ray.Origin + ray.Direction * hit.Tdist;
		return hit;
	}

	Plane::Plane(Vec3& a_PN, float a_D)
		: m_Normal(a_PN), m_Distance(a_D)
	{
	}

	Plane::Plane(Vec3& P0, Vec3& P1, Vec3& P2)
	{
		CalculateAABB(P0, P1, P2);
		//Calculate the normal plane: counter-clockwise vectorial product.
		m_Normal = CrossProduct((P1 - P0), (P2 - P0));
		
		if ((m_Normal.SqrMagnitude()) == 0.0)
		{
			std::cerr << "DEGENERATED PLANE!\n";
		}
		else
		{
			m_Normal.Normalized();
			m_Distance = m_Normal.DotProduct(P0);
		}
		
	}

	//
	// Ray/Plane intersection test.
	//
	RayCastHit Plane::Intercepts(Ray& r)
	{
		Vec3 pointA = m_Normal * m_Distance;

		float projection = DotProduct(r.Direction, m_Normal);
		//Ray and plane are perpendicular
		if (projection == 0)
		{
			return RayCastHit(false);
		}

		float t = -DotProduct((r.Origin - pointA), m_Normal) / projection;
		if (t > EPSILON)
		{
			return RayCastHit(true, t, this, r.Origin + t * r.Direction);
		}
		return RayCastHit(false);
	}

	void Plane::CalculateAABB(Vec3& P0, Vec3& P1, Vec3& P2)
	{
		m_BoundingBox.Min.x = std::min({ P0.x, P1.x, P2.x });
		m_BoundingBox.Min.y = std::min({ P0.y, P1.y, P2.y });
		m_BoundingBox.Min.z = std::min({ P0.z, P1.z, P2.z });

		m_BoundingBox.Max.x = std::max({ P0.x, P1.x, P2.x });
		m_BoundingBox.Max.y = std::max({ P0.y, P1.y, P2.y });
		m_BoundingBox.Max.z = std::max({ P0.z, P1.z, P2.z });
	}


	RayCastHit Sphere::Intercepts(Ray& ray)
	{
		// center and origin inversed and signs of b inversed for sphere optimization
		Vec3 temp = m_Center - ray.Origin;

		float b = DotProduct(temp, ray.Direction);
		float c = DotProduct(temp, temp) - m_Radius * m_Radius;
		
		// If origin outside and pointing away from sphere
		if (c > 0 && b <= 0)
			return { false };

		float a = 1;// length of ray.Direction should be 1 
		float disc = b * b - a * c;

		if (disc <= 0.0)
			return { false };

		float e = sqrtf(disc);
		float denom = a;
		float t = (b - e) / denom; // root 1

		if (t > EPSILON)
			return { true, t,  this, ray.Origin + t * ray.Direction };

		t = (b + e) / denom; //root 2

		if (t > EPSILON)
			return { true, t,  this, ray.Origin + t * ray.Direction };
			//return { true, t, ray.Origin + t * ray.Direction, (temp + t * ray.Direction) / m_Radius, m_Material };

		return { false };
	}


	Vec3 Sphere::GetNormal(Vec3 point)
	{
		Vec3 normal = point - m_Center;
		return normal.Normalized();
	}

	void Sphere::CalculateAABB()
	{	
		Vec3 min = {m_Center.x - m_Radius, m_Center.y - m_Radius, m_Center.z - m_Radius};
		Vec3 max = {m_Center.x + m_Radius, m_Center.y + m_Radius, m_Center.z + m_Radius};
		m_BoundingBox = { min , max };
	}

	/// <summary>
	/// Only valid if the object intersected with ray
	/// </summary>
	/// <param name="contactPoint"></param>
	/// <param name="ray"></param>
	/// <returns></returns>
	bool Sphere::IsInsideObject(const Vec3& contactPoint, const Ray& ray)
	{
		// contactPoint was inside object if point-center vector is pointing to same hemisphere as ray
		return ((contactPoint - m_Center).DotProduct(ray.Direction)) > 0;
	}

	Box::Box(Vec3& minPoint, Vec3& maxPoint) //Axis aligned Box: another geometric object
		: m_BoundingBox(minPoint, maxPoint), m_Center((minPoint + maxPoint) * 0.5f)
	{

	}

	RayCastHit Box::Intercepts(Ray& ray)
	{
		Vec3 dirfrac;
		// r.dir is unit direction vector of ray
		dirfrac.x = 1.0f / ray.Direction.x;
		dirfrac.y = 1.0f / ray.Direction.y;
		dirfrac.z = 1.0f / ray.Direction.z;
		// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
		// r.org is origin of ray
		const Vec3& lb = m_BoundingBox.Min;
		const Vec3& rt = m_BoundingBox.Max;
		float t1 = (lb.x - ray.Origin.x) * dirfrac.x;
		float t2 = (rt.x - ray.Origin.x) * dirfrac.x;
		float t3 = (lb.y - ray.Origin.y) * dirfrac.y;
		float t4 = (rt.y - ray.Origin.y) * dirfrac.y;
		float t5 = (lb.z - ray.Origin.z) * dirfrac.z;
		float t6 = (rt.z - ray.Origin.z) * dirfrac.z;

		// find largest entering t value
		float tmin = std::max({ std::min(t1, t2), std::min(t3, t4), std::min(t5, t6) });
		// find smallest exiting t value
		float tmax = std::min({ std::max(t1, t2), std::max(t3, t4), std::max(t5, t6) });

		// Condition for a hit
		// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
		if (tmax < EPSILON)
			return {false, tmax};

		// if tmin > tmax, ray doesn't intersect AABB
		if (tmin > tmax)
			return { false, tmax };

		return { true, tmin, this,  ray.Origin + tmin * ray.Direction };
	}

	Vec3 Box::GetNormal(Vec3 point)
	{
		// TODO maybe a possible aproach to avoid this calculus could be the addition of the normal
		// ---  to the RayCastHit class
		Vec3 hitToCenterDir = point - m_Center;
		Vec3 d =  (m_BoundingBox.Max - m_BoundingBox.Min) * 0.5f;
		
		constexpr float epsilon = 0.9999f;
		float x = abs((hitToCenterDir.x * (1 / d.x)));
		// This comparation is to avoid the clamping of the value to zero due the floating point precission issue
		x = x >= epsilon ? 1 : 0;
		float y = abs((hitToCenterDir.y * (1 / d.y)));
		y = y >= epsilon ? 1 : 0;
		float z = abs((hitToCenterDir.z * (1 / d.z)));
		z = z >= epsilon ? 1 : 0;
		Vec3 normal = Vec3(x, y, z);
						  
		return normal.Normalized();
		
	}

	bool Box::IsInsideObject(const Vec3& point, const Ray& ray)
	{
		// TODO maybe a possible aproach to avoid this calculus could be the addition of the normal
		// ---  to the RayCastHit class
		Vec3 dirfrac;
		// r.dir is unit direction vector of ray
		dirfrac.x = 1.0f / ray.Direction.x;
		dirfrac.y = 1.0f / ray.Direction.y;
		dirfrac.z = 1.0f / ray.Direction.z;
		// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
		// r.org is origin of ray
		const Vec3& lb = m_BoundingBox.Min;
		const Vec3& rt = m_BoundingBox.Max;
		float t1 = (lb.x - ray.Origin.x) * dirfrac.x;
		float t2 = (rt.x - ray.Origin.x) * dirfrac.x;
		float t3 = (lb.y - ray.Origin.y) * dirfrac.y;
		float t4 = (rt.y - ray.Origin.y) * dirfrac.y;
		float t5 = (lb.z - ray.Origin.z) * dirfrac.z;
		float t6 = (rt.z - ray.Origin.z) * dirfrac.z;

		// find largest entering t value
		float tmin = std::max({ std::min(t1, t2), std::min(t3, t4), std::min(t5, t6) });
		// find smallest exiting t value
		float tmax = std::min({ std::max(t1, t2), std::max(t3, t4), std::max(t5, t6) });

		// Condition for a hit
		if (tmin < tmax && tmax > EPSILON)
		{
			if (tmin > EPSILON) // Ray hits outside
				return false;
			else // Ray hits inside
				return true;

		}

		return false;
	}

#pragma region BubbleSphere
	BubbleSphere::BubbleSphere(Vec3& a_center, float a_radiusfloat, float _perlingFreq, float bubbleLowLimit, Material* _childMaterial) : Sphere(a_center, a_radiusfloat)
	{
		m_BubbleThreshold = bubbleLowLimit;
		m_PerlinSampingFreq = _perlingFreq;
		m_ChildMaterial = _childMaterial;
		GenerateChildSpheres();
	}

	std::vector<Sphere*> BubbleSphere::GetChildSpheres()
	{
		return m_ChildSpheres;
	}

	void BubbleSphere::SetSampleStep(float step)
	{
		m_Step = step;
		GenerateChildSpheres();
	}

	void BubbleSphere::SetBubbleThreshold(float threshold)
	{
		m_BubbleThreshold = threshold;
		GenerateChildSpheres();
	}

	void BubbleSphere::SetPerlinSamplingFreq(float frequency)
	{
		m_PerlinSampingFreq = frequency;
		GenerateChildSpheres();
	}

	void BubbleSphere::GenerateChildSpheres()
	{
		siv::PerlinNoise perlin = siv::PerlinNoise(time(NULL));

		// Maps each a set of sphere centers to (x,y,diameter)
		// These will later be compared and for each sphere center, the coordinate with the largest diameter prevails
		std::map<float, std::vector<Vec3>> CoordSphereCenters = std::map<float, std::vector<Vec3>>();

		bool inSphere = false;
		float enterZ = 0.0f;

		// Step 1: Sampling/Mapping 
		for (float x = -1; x <= (1 + EPSILON); x += m_Step)
		{
			for (float y = -1; y <= (1 + EPSILON); y += m_Step)
			{
				for (float z = -1; z <= (1 + EPSILON); z += m_Step)
				{
					if ((x * x + y * y + z * z) > 1)
						continue; // Keep inside sphere

					Vec3 samplePos = m_Center + Vec3(x, y, z) * this->m_Radius;
					float value = perlin.noise3D_0_1(samplePos.x * m_PerlinSampingFreq, samplePos.y * m_PerlinSampingFreq, samplePos.z * m_PerlinSampingFreq);
					if (value >= this->m_BubbleThreshold)
					{
						if (!inSphere)
						{
							// Entering sphere
							inSphere = true;
							enterZ = samplePos.z;
						}
					}
					else if (inSphere)
					{
						// Exiting sphere
						inSphere = false;

						float diameter = (m_Center.z + z * m_Radius) - enterZ;
						float center = enterZ + (diameter / 2);

						if (CoordSphereCenters.count(center) == 0)
							CoordSphereCenters[center] = std::vector<Vec3>();

						CoordSphereCenters[center].push_back(Vec3(samplePos.x, samplePos.y, diameter));
					}

					// If next step would reset iteration - end child sphere
					float nextZ = z + m_Step;
					if (((fabs(z - 1) < EPSILON) || ((x * x + y * y + nextZ * nextZ) > 1)) && inSphere)
					{
						// Exiting child sphere
						inSphere = false;

						float diameter = (m_Center.z + z * m_Radius) - enterZ;
						if (diameter <= EPSILON)
							continue;

						float center = enterZ + (diameter / 2);

						if (CoordSphereCenters.count(center) == 0)
							CoordSphereCenters[center] = std::vector<Vec3>();

						CoordSphereCenters[center].push_back(Vec3(samplePos.x, samplePos.y, diameter));
					}
				}
			}
		}

		// Step 2: Finding spheres
		std::map<float, Vec3> Spheres = std::map<float, Vec3>();
		for each (auto & centerToCoordDiameters in CoordSphereCenters)
		{
			Spheres[centerToCoordDiameters.first] = centerToCoordDiameters.second[0];
			for each (Vec3 centerDiam in centerToCoordDiameters.second)
			{
				// If diameter larger than current diameter - replace
				if (centerDiam.z > Spheres[centerToCoordDiameters.first].z)
					Spheres[centerToCoordDiameters.first] = centerDiam;
			}
		}

		// Step 3: Building spheres
		for each (auto & sphere in Spheres)
		{
			float radius = sphere.second.z / 2;
			float xPos = sphere.second.x;
			float yPos = sphere.second.y;
			float zPos = sphere.first; //m_Center.z + sphere.first * radius;
			Sphere* child = new Sphere(Vec3(xPos, yPos, zPos), radius);
			child->SetMaterial(m_ChildMaterial);
			m_ChildSpheres.push_back(child);
		}
	}

	RayCastHit BubbleSphere::Intercepts(Ray& ray)
	{
		// center and origin inversed and signs of b inversed for sphere optimization
		Vec3 temp = m_Center - ray.Origin;

		float b = DotProduct(temp, ray.Direction);
		float c = DotProduct(temp, temp) - m_Radius * m_Radius;

		// If origin outside and pointing away from sphere
		if (c > 0 && b <= 0)
			return { false };

		float a = 1;// length of ray.Direction should be 1 
		float disc = b * b - a * c;

		if (disc <= 0.0)
			return { false };

		float e = sqrtf(disc);
		float denom = a;
		float t = (b - e) / denom; // root 1

		if (t > EPSILON)
		{
			RayCastHit parentHit = RayCastHit(true, t, this, ray.Origin + t * ray.Direction);
			RayCastHit childHit = this->GetClosestHitInChildSpheres(ray, t);
			return childHit ? childHit : parentHit;
		}

		t = (b + e) / denom; //root 2

		if (t > EPSILON)
		{
			RayCastHit parentHit = RayCastHit(true, t, this, ray.Origin + t * ray.Direction);
			RayCastHit childHit = this->GetClosestHitInChildSpheres(ray, t);
			return childHit ? childHit : parentHit;
		}

		return { false };
	}

	RayCastHit BubbleSphere::GetClosestHitInChildSpheres(Ray& ray, float tmin)
	{
		RayCastHit hit, temphit;
		for (auto& sphere : m_ChildSpheres)
		{
			temphit = sphere->Intercepts(ray);
			if (temphit && temphit.Tdist < tmin && (temphit.InterceptionPoint - this->m_Center).SqrMagnitude() < this->SqRadius)
			{
				tmin = temphit.Tdist;
				hit = temphit;
			}
		}
		return hit;
	}

	std::ostream& operator<<(std::ostream& stream, const BubbleSphere& sphere)
	{
		// TODO: insert return statement here
		stream << "Bubble Sphere : {\n";
		stream << "		Parent : Center = " << sphere.m_Center << "		Radius = " << sphere.m_Radius << std::endl;
		stream << "		Children : [\n";
		for each (auto& child in sphere.m_ChildSpheres)
		{
			stream << "			Center = " << child->GetCenter() << "	Radius = " << child->GetRadius() << std::endl;
		}
		stream << "		]\n";
		stream << "}\n";
		return stream;
	}

#pragma endregion BubbleSphere
}