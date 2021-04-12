#pragma once
#include "vector.h"
#include "ray.h"
#include "Math/Maths.h"

namespace rayTracer
{
	struct AABB
	{
		Vec3 Min = { };
		Vec3 Max = { };

		AABB();
		AABB(const Vec3& min, const Vec3& max)
			: Min(min), Max(max) {}
		AABB(const AABB& bbox)
			: Min(bbox.Min), Max(bbox.Max){}
		virtual ~AABB() = default;


		AABB operator = (const AABB& rhs);
		bool IsInside(const Vec3& p);
		bool intercepts(const Ray& r, float& t);
		Vec3 centroid();
		void extend(AABB box);
	};
}
