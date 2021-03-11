#include "SceneObject.h"
#include "Core/Constant.h"
#include <algorithm>
namespace rayTracer
{

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
		Vec3 N = CrossProduct(Edge1, Edge2);
		float det = -DotProduct(ray.Direction, N);
		float invdet = 1.0f / det;
		Vec3 AO = ray.Origin - m_Points[0];
		Vec3 DAO = CrossProduct(AO, ray.Direction);
		float u = DotProduct(Edge2, DAO) * invdet;
		float v = -DotProduct(Edge1, DAO) * invdet;

		RayCastHit hit;
		hit.Tdist = DotProduct(AO, N) * invdet;
		hit.Hit = (det >= 1e-6 && hit.Tdist >= 0.0 && u >= 0.0 && v >= 0.0 && (u + v) <= 1.0f);
		hit.Object = this;
		hit.InterceptionPoint = ray.Origin + ray.Direction * hit.Tdist;
		return hit;
	}
	/** /
	Plane::Plane(Vector& a_PN, float a_D)
		: PN(a_PN), D(a_D)
	{

	}
	/**/
	Plane::Plane(Vec3& P0, Vec3& P1, Vec3& P2)
	{
		m_Vertices[0] = P0;
		m_Vertices[1] = P1;
		m_Vertices[2] = P2;

		//Calculate the normal plane: counter-clockwise vectorial product.
		m_Normal = CrossProduct((P1 - P0), (P2 - P1)); // % is cross product, for some reason.
		m_Normal.Normalized();
		/** /
		if ((l = m_Normal.length()) == 0.0)
		{
			std::cerr << "DEGENERATED PLANE!\n";
		}
		else
		{
			m_Normal.normalize();
			// TODO: CHECK IF NECESSARY
			//Calculate D
			//D = m_Normal.x * P0.x + m_Normal.y * P0.y + m_Normal.z * P0.z;
		}
		/**/
	}

	//
	// Ray/Plane intersection test.
	//
	RayCastHit Plane::Intercepts(Ray& r)
	{
		//Ray and plane are perpendicular
		if (DotProduct(r.Direction, m_Normal) == 0)
		{
			return RayCastHit(false);
		}

		float t = -DotProduct((r.Origin - m_Vertices[0]), m_Normal) / DotProduct(m_Normal, r.Direction);
		if (t > EPSILON)
		{
			return RayCastHit(true, t, this, r.Origin + t * r.Direction);
		}
		return RayCastHit(false);
	}


	RayCastHit Sphere::Intercepts(Ray& ray)
	{
		
		Vec3 temp = ray.Origin - m_Center;
		float a = DotProduct(ray.Direction, ray.Direction); // length of ray.Direction. should be 1 
		float b = 2.0f * DotProduct(temp, ray.Direction);
		float c = DotProduct(temp, temp) - m_Radius * m_Radius;
		float disc = b * b - 4.0f * a * c;

		if (disc < 0.0)
			return { false };

		float e = sqrtf(disc);
		float denom = 2.0f * a;
		float t = (-b - e) / denom; // root 1

		if (t > EPSILON)
			return { true, t,  this, ray.Origin + t * ray.Direction };

		t = (-b + e) / denom; //root 2

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

	aaBox::aaBox(Vec3& minPoint, Vec3& maxPoint) //Axis aligned Box: another geometric object
		: m_BoundingBox(minPoint, maxPoint)
	{
	}

	RayCastHit aaBox::Intercepts(Ray& ray)
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

		float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
		float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

		RayCastHit hit;
		// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
		if (tmax < EPSILON)
			return {false, tmax};

		// if tmin > tmax, ray doesn't intersect AABB
		if (tmin > tmax)
			return { false, tmax };

		return { true, tmin, this,  ray.Origin + tmin * ray.Direction};
	}
}