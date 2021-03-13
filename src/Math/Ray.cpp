#include "Math/Ray.h"
#include "Scene/SceneObject.h"
#include "Core/Constant.h"

namespace rayTracer
{

	Ray RayCastHit::CalculateRefractedRay(const Vec3& normal, const RayCastHit& hit, const Vec3& viewDir, float incidentRefractionIndex, float exitRefractIndex)
	{
		Vec3 invViewDir = -1 * viewDir;
		Vec3 emissionPoint = hit.InterceptionPoint - normal * DISPLACEMENT_BIAS;
		Vec3 refractionDir = Refract(viewDir, normal, incidentRefractionIndex / exitRefractIndex);
		// Testing other method
		//float incidentAngle = acosf(normal.DotProduct(invViewDir));
		//float sinIncidentAngle = sinf(incidentAngle);
		//float sinExitAngle = std::clamp(incidentRefractionIndex * sinIncidentAngle / exitRefractIndex, -1.0f, 1.0f);
		//float cosExitAngle = cosf(asinf(sinExitAngle));
		//Vec3 parallelVector = ((invViewDir.DotProduct(normal)) * normal) - invViewDir;
		//parallelVector = parallelVector / sinIncidentAngle; // (Normalize)	
		//refractionDir = sinExitAngle * parallelVector + cosExitAngle * (-1 * normal);
		//
		return { emissionPoint, refractionDir };
	}
	Ray RayCastHit::CalculateReflectedRay(const RayCastHit& hit, const Vec3& dir)
	{
		Vec3 normal = hit.Object->GetNormal(hit.InterceptionPoint);
		Vec3 emissionPoint = hit.InterceptionPoint + normal * DISPLACEMENT_BIAS;
		Vec3 reflectionDir = Reflect(dir, normal);
		return { emissionPoint , reflectionDir };
	}
}