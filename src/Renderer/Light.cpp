#include "Light.h"
#include "Renderer/SceneRenderer.h"
#include "Math/Maths.h"

namespace rayTracer
{
	float Light::GetIntensityAtPoint(const Vec3& emissionPoint, const std::vector<Object*>& objectsInScene, AntialiasingMode antialiasingMode)
	{
		if (!shadows)
			return 1;

		Vec3 lightDir = Vec3(this->position - emissionPoint);
		float lightDistance = lightDir.Magnitude();//Light should not be blocked by objects behind the light source
		lightDir = lightDir.Normalized();
		Ray shadowFeeler(emissionPoint, lightDir);

		for (auto obj : objectsInScene)
		{
			if (obj->GetMaterial()->GetTransmittance() > 0)
				continue; // Transparent objects do not block light (Should refract light, but....)

			RayCastHit shadowHit = obj->Intercepts(shadowFeeler);
			if (shadowHit && shadowHit.Tdist < lightDistance)
				return 0;
		}
		return 1;
	}


	AreaLight::AreaLight(const Vec3& pos, const Vec3& col, const Vec3& side1, const Vec3& side2) : Light(pos, col)
	{
		sideA = side1;
		sideB = side2;
		cornerPos = pos - 0.5 * sideA - 0.5 * sideB;
		sqrdNbPoints = 4;
	}

	AreaLight::AreaLight(const Vec3& pos, const Vec3& col, const Vec3& side1, const Vec3& side2, int _sqrdNbPoints) : Light(pos, col)
	{
		sideA = side1;
		sideB = side2;
		cornerPos = pos - 0.5 * sideA - 0.5 * sideB;
		sqrdNbPoints = _sqrdNbPoints;
	}

	float AreaLight::GetIntensityAtPoint(const Vec3& emissionPoint, const std::vector<Object*>& objectsInScene, AntialiasingMode antialiasingMode)
	{
		if (!shadows)
			return 1;

		float lightDistance = (this->position - emissionPoint).Magnitude();//Light should not be blocked by objects behind the light source
		
		if (antialiasingMode == AntialiasingMode::NONE)
		{
			float intensity = (sqrdNbPoints * sqrdNbPoints);
			float distBetweenPoints = 1.0f / (sqrdNbPoints + 1);
			for (int i = 0; i < sqrdNbPoints; i++)
			{
				for (int j = 0; j < sqrdNbPoints; j++)
				{
					float alpha = (i + 1) * distBetweenPoints;
					float beta = (j + 1) * distBetweenPoints;
					Vec3 position = this->cornerPos + alpha * this->sideA + beta * this->sideB;

					Vec3 lightDir = Normalize(position - emissionPoint);
					Ray shadowFeeler(emissionPoint, lightDir);

					for (auto obj : objectsInScene)
					{
						if (obj->GetMaterial()->GetTransmittance() > 0)
							continue; // Transparent objects do not block light (Should refract light, but....)

						RayCastHit shadowHit = obj->Intercepts(shadowFeeler);
						if (shadowHit && shadowHit.Tdist < lightDistance)
						{
							intensity -= 1.0f;
							break;
						}
					}
				}
			}
			return intensity / (sqrdNbPoints * sqrdNbPoints);
		}
		else 
		{
			// Multi-sampling handling
			int samplingCount = SceneRenderer::GetLightSamplingArray().size();

			// New pixel, restart sampling index
			if (samplingIndex >= samplingCount)
				samplingIndex = 0;
			
			Vec2 samplingOffset = SceneRenderer::GetLightSamplingArray()[samplingIndex];
			samplingIndex++;

			float intensity = 1;
			Vec3 position = this->cornerPos + samplingOffset.x * this->sideA + samplingOffset.y * this->sideB;

			Vec3 lightDir = Normalize(position - emissionPoint);
			Ray shadowFeeler(emissionPoint, lightDir);

			for (auto obj : objectsInScene)
			{
				if (obj->GetMaterial()->GetTransmittance() > 0)
					continue; // Transparent objects do not block light (Should refract light, but....)

				RayCastHit shadowHit = obj->Intercepts(shadowFeeler);
				if (shadowHit && shadowHit.Tdist < lightDistance)
					return 0.0f;
			}

			return 1.0f;
		}
	}
}