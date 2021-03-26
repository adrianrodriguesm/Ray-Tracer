#include "Light.h"

#include "Math/Maths.h"

namespace rayTracer
{
	float Light::GetIntensityAtPoint(const Vec3& emissionPoint, const std::vector<Object*>& objectsInScene, AntialiasingMode antialiasingMode)
	{
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
		if (antialiasingMode == AntialiasingMode::NONE)
		{
			float intensity = (sqrdNbPoints * sqrdNbPoints);
			float distBetweenPoints = 1.0f / (sqrdNbPoints + 1);
			float lightDistance = (this->position - emissionPoint).Magnitude();//Light should not be blocked by objects behind the light source
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
			std::cout << "\nERROR: Shadows are not yet supported for multi-sampling\n";
			exit(EXIT_FAILURE);

			//set_rand_seed(time(nullptr) + i);
			//float alpha = rand_float();
			//float beta = rand_float();
			//Vec3 position = this->cornerPos + alpha * this->sideA + beta * this->sideB;
		}
	}
}