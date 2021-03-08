#pragma once
#include "Vector.h"

namespace rayTracer
{
	class Object;
	struct Ray
	{
		Vector Origin;
		Vector Direction;
		Ray() = default;
		Ray(const Vector& o, const Vector& dir) : Origin(o), Direction(dir) {};
	};

	struct RayCastHit
	{
		bool Hit = false;
		float Tdist = 0.f;
		Object* Object = nullptr;
		Vector InterceptionPoint = {};
		RayCastHit() = default;
		RayCastHit(bool hit, float t = 0, rayTracer::Object* object = nullptr, const Vector& interceptionPoint = { 0,0,0 })
			: Tdist(t), Hit(hit), Object(object), InterceptionPoint(interceptionPoint) {}

		operator bool() { return Hit; }
	};
}
