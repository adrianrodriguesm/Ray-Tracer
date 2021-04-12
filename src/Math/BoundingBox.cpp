#include "BoundingBox.h"

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
	Vec3 AABB::centroid() {
		return (Min + Max) / 2;
	}

	// --------------------------------------------------------------------- extend AABB
	void AABB::extend(AABB box) {
		if (Min.x > box.Min.x) Min.x = box.Min.x;
		if (Min.y > box.Min.y) Min.y = box.Min.y;
		if (Min.z > box.Min.z) Min.z = box.Min.z;

		if (Max.x < box.Max.x) Max.x = box.Max.x;
		if (Max.y < box.Max.y) Max.y = box.Max.y;
		if (Max.z < box.Max.z) Max.z = box.Max.z;
	}

	// --------------------------------------------------------------------- AABB intersection

	bool AABB::intercepts(const Ray& ray, float& t)
	{
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
	}
}