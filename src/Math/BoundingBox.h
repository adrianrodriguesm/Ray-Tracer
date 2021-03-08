#pragma once
#include "vector.h"
#include "ray.h"

namespace rayTracer
{
	struct AABB
	{
		Vector Min = {-1.0f, -1.0f, -1.0f };
		Vector Max = { 1.0f,  1.0f,  1.0f };

		AABB() = default;
		AABB(const Vector& min, const Vector& max)
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
		bool IsInside(const Vector& p)
		{
			return ((p.x > Min.x && p.x < Max.x) && (p.y > Min.y && p.y < Max.y) && (p.z > Min.z && p.z < Max.z));
		}
	};
}
