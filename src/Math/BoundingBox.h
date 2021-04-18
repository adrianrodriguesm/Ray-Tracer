#pragma once
#include "Math/Ray.h"
#include "Core/Constant.h"
namespace rayTracer
{
	struct AABB
	{
		Vec3 Min = FLOAT_MAX;
		Vec3 Max = FLOAT_MIN;

		AABB();
		AABB(const Vec3& min, const Vec3& max)
			: Min(min), Max(max) {}
		AABB(const AABB& bbox)
			: Min(bbox.Min), Max(bbox.Max){}
		virtual ~AABB() = default;


		AABB operator = (const AABB& rhs);
		bool IsInside(const Vec3& p);
		bool Intercepts(const Ray& r, float& t);
		Vec3 Centroid();
		void Extend(AABB box);
	};
}
