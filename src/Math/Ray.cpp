#include "Math/Ray.h"
#include "Scene/SceneObject.h"
namespace rayTracer
{
	Ray RayCastHit::CalculateRefractedRay(const RayCastHit& hit, const Ray& ray, float incidentRefractionIndex, float materialRefractionIndex)
	{
		Vec3 normal = hit.Object->GetNormal(hit.InterceptionPoint);
		Vec3 refract = Refract(ray.Direction, normal, incidentRefractionIndex / materialRefractionIndex);
		return { hit.InterceptionPoint, refract };
	}
	Ray RayCastHit::CalculateReflectedRay(const RayCastHit& hit, const Vec3& dir)
	{
		Vec3 normal = hit.Object->GetNormal(hit.InterceptionPoint);
		Vec3 reflectionDir = Reflect(dir, normal);
		return { hit.InterceptionPoint , reflectionDir };
	}
}