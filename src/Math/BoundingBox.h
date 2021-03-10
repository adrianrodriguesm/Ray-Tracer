#pragma once
#include "vector.h"
#include "ray.h"

namespace rayTracer
{
	struct AABB
	{
		Vec3 Min = {-1.0f, -1.0f, -1.0f };
		Vec3 Max = { 1.0f,  1.0f,  1.0f };

		AABB() = default;
		AABB(const Vec3& min, const Vec3& max)
			: Min(min), Max(max) {}
		AABB(const AABB& bbox)
			: Min(bbox.Min), Max(bbox.Max){}

		virtual ~AABB() = default;

		AABB operator = (const AABB& rhs)
		{
			if (this == &rhs)
				return (*this);
			Min = rhs.Min;
			Max = rhs.Max;
			return (*this);
		}
		// --------------------------------------------------------------------- inside
		// used to test if a ray starts inside a grid
		inline bool IsInside(const Vec3& p)
		{
			return ((p.x > Min.x && p.x < Max.x) && (p.y > Min.y && p.y < Max.y) && (p.z > Min.z && p.z < Max.z));
		}
	};
}
