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
		/** /
		Ray calculateRefractedRay(Hit hit, Ray ray, Material* mat, float RefractionIndex) {
			Vector3 Nrefr = hit.Normal.normalize();
			Vector3 I = (hit.Location - ray.O).normalize();
			float NdotI = Nrefr * I;
			NdotI = MathUtils::clamp(NdotI, 1, -1);
			float etai = RefractionIndex, etat = mat->refractionIndex;
			if (NdotI < 0) {
				//outside the surface, cos theta should be positive
				NdotI = -NdotI;
			}
			else {
				//inside the surface, reverse normal direction
				Nrefr = -hit.Normal.normalize();
				std::swap(etai, etat);
			}

			float eta = etai / etat;
			float k = 1 - ((eta * eta) * (1 - NdotI * NdotI));
			if (k < 0) {
				return Ray(hit.Location, Vector3(0, 0, 0));
			}
			else
				return Ray(hit.Location, eta * (I)+(eta * NdotI - sqrtf(k)) * Nrefr);

		}
		/**/
		// View Dir Need to be normalize
		static Ray CalculateReflectedRay(const RayCastHit& hit, const Vec3& dir);
		/** /
		Ray CalculateReflectedRay(RayCastHit& hit, Vec3& viewPosition)
		{
			Vec3 viewDir = Vec3(hit.InterceptionPoint - viewPosition).Normalized();
			Vec3 normal = hit.Object->GetNormal(hit.InterceptionPoint);
			Vec3 reflectionDir = Reflect(-viewDir, normal);
			return { hit.InterceptionPoint , reflectionDir };
		}
		/**/
	};
}
