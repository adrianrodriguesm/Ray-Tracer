#pragma once
#include "Vector.h"
#include <algorithm>
namespace rayTracer
{
	class Object;
	struct Ray
	{
		Vec3 Origin;
		Vec3 Direction;
		Ray() = default;
		Ray(const Vec3& o, const Vec3& dir) : Origin(o), Direction(dir) {};
	};

	struct RayCastHit
	{
		bool Hit = false;
		float Tdist = 0.f;
		rayTracer::Object* Object = nullptr;
		Vec3 InterceptionPoint = {};
		RayCastHit() = default;
		RayCastHit(bool hit, float t = 0, rayTracer::Object* object = nullptr, const Vec3& interceptionPoint = { 0,0,0 })
			: Tdist(t), Hit(hit), Object(object), InterceptionPoint(interceptionPoint) {}

		operator bool() { return Hit; }

		static Ray CalculateRefractedRay(const RayCastHit& hit, const Ray& ray, float incidentRefractionIndex, float materialRefractionIndex);
		// View Dir Need to be normalize
		static Ray CalculateReflectedRay(const RayCastHit& hit, const Vec3& dir);
	};
}