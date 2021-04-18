#include "pch.h"
#include "BoundingBox.h"
#include "Core/Constant.h"
namespace rayTracer
{
	
	//-------------------------------------------------------------------- - default constructor
	AABB::AABB()
	{
		Min = Vec3(-1.0f, -1.0f, -1.0f);
		Max = Vec3(1.0f, 1.0f, 1.0f);
	}
	// --------------------------------------------------------------------- assignment operator
	AABB AABB::operator= (const AABB& rhs) 
	{
		if (this == &rhs)
			return (*this);
		Min = rhs.Min;
		Max = rhs.Max;
		return (*this);
	}

	// --------------------------------------------------------------------- inside
	// used to test if a ray starts inside a grid
	bool AABB::IsInside(const Vec3& p)
	{
		return ((p.x > Min.x && p.x < Max.x) && (p.y > Min.y && p.y < Max.y) && (p.z > Min.z && p.z < Max.z));
	}
	
	// --------------------------------------------------------------------- compute centroid
	Vec3 AABB::Centroid() 
	{
		return (Min + Max) / 2;
	}

	// --------------------------------------------------------------------- extend AABB
	void AABB::Extend(AABB box) {
		if (Min.x > box.Min.x) Min.x = box.Min.x;
		if (Min.y > box.Min.y) Min.y = box.Min.y;
		if (Min.z > box.Min.z) Min.z = box.Min.z;

		if (Max.x < box.Max.x) Max.x = box.Max.x;
		if (Max.y < box.Max.y) Max.y = box.Max.y;
		if (Max.z < box.Max.z) Max.z = box.Max.z;
	}

	// --------------------------------------------------------------------- AABB intersection
	bool AABB::Intercepts(const Ray& ray, float& t)
	{
		/**/
		Vec3 dirfrac;
		// r.dir is unit direction vector of ray
		dirfrac.x = 1.0f / ray.Direction.x;
		dirfrac.y = 1.0f / ray.Direction.y;
		dirfrac.z = 1.0f / ray.Direction.z;
		// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
		// r.org is origin of ray
		const Vec3& lb = Min;
		const Vec3& rt = Max;
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
			return  false;

		// if tmin > tmax, ray doesn't intersect AABB
		if (tmin > tmax)
			return false;

		t = tmin;
		return true;
		/** /
		double t0, t1;

		float ox = ray .Origin.x;
		float oy = ray.Origin.y;
		float oz = ray.Origin.z;
		float dx = ray.Direction.x;
		float dy = ray.Direction.y;
		float dz = ray.Direction.z;

		float x0 = Min.x;
		float y0 = Min.y;
		float z0 = Min.z;
		float x1 = Max.x;
		float y1 = Max.y;
		float z1 = Max.z;

		float tx_Min, ty_Min, tz_Min;
		float tx_Max, ty_Max, tz_Max;

		float a = 1.0 / dx;
		if (a >= 0) {
			tx_Min = (x0 - ox) * a;
			tx_Max = (x1 - ox) * a;
		}
		else {
			tx_Min = (x1 - ox) * a;
			tx_Max = (x0 - ox) * a;
		}

		float b = 1.0 / dy;
		if (b >= 0) {
			ty_Min = (y0 - oy) * b;
			ty_Max = (y1 - oy) * b;
		}
		else {
			ty_Min = (y1 - oy) * b;
			ty_Max = (y0 - oy) * b;
		}

		float c = 1.0 / dz;
		if (c >= 0) {
			tz_Min = (z0 - oz) * c;
			tz_Max = (z1 - oz) * c;
		}
		else {
			tz_Min = (z1 - oz) * c;
			tz_Max = (z0 - oz) * c;
		}

		//largest entering t value
		t0 = MAX3(tx_Min, ty_Min, tz_Min);

		//smallest exiting t value
		t1 = MIN3(tx_Max, ty_Max, tz_Max);

		t = (t0 < 0) ? t1 : t0;

		return (t0 < t1&& t1 > 0);
		/**/
	}
}