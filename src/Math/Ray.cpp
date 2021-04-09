#include "Math/Ray.h"
#include "Scene/SceneObject.h"
#include "Core/Constant.h"
#include "Math/Random.h"
#include "Renderer/SceneRenderer.h"
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
		// Objects without roughness
		if (hit.Object->GetMaterial()->GetRoughness() <= 0)
			return { emissionPoint , reflectionDir };
		/**/
		Vec3 spherePoint = SceneRenderer::SampleHemisphere();
		Vec3 reflectionFuzzyDir = Normalize(reflectionDir + hit.Object->GetMaterial()->GetRoughness() * spherePoint);
		if (reflectionFuzzyDir.DotProduct(hit.Object->GetNormal(hit.InterceptionPoint)) < 0)
		{
			spherePoint.x *= -1.f;
			spherePoint.y *= -1.f;
			reflectionDir = Normalize(reflectionDir + hit.Object->GetMaterial()->GetRoughness() * spherePoint);
		}
		else
		{
			return { emissionPoint , reflectionFuzzyDir };
		}
		/** /
		Vec3 w = reflectionDir;
		Vec3 u = (CrossProduct(Vec3(0.00424f, 1, 0.00764f), w)).Normalized();
		Vec3 v = CrossProduct(u, w);

		Vec3 hemispherePoint = SceneRenderer::SampleHemisphere() * hit.Object->GetMaterial()->GetRoughness();
		reflectionDir = hemispherePoint.x * u + hemispherePoint.y * v + hemispherePoint.z * w;

		if (hit.Object->GetNormal(hit.InterceptionPoint).DotProduct(reflectionDir) < 0.f)
			reflectionDir = -hemispherePoint.x * u - hemispherePoint.y * v + hemispherePoint.z * w;
		/**/
		return { emissionPoint , reflectionDir };
	}
}