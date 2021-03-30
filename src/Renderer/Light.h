#pragma once
#include "Math/Vector.h"
#include "Scene/SceneObject.h"
#include "Renderer/AntiAliasing.h"
#include <vector>

namespace rayTracer
{
	class Light
	{
	public:

		Light(const Vec3& pos, const Vec3& col) : position(pos), color(col) {};
		virtual ~Light() = default;
		Vec3 position;
		Vec3 color;

		virtual float GetIntensityAtPoint(const Vec3& EmissionPoint, const std::vector<Object*>& objectsInScene, AntialiasingMode antialiasingMode);
	};

	class AreaLight : public Light
	{
	private:
		// Variable for shadow sampling with antialiasing
		int samplingIndex = 0;

	public:
		AreaLight(const Vec3& pos, const Vec3& col, const Vec3& side1, const Vec3& side2);
		AreaLight(const Vec3& pos, const Vec3& col, const Vec3& side1, const Vec3& side2, int sqrdNbPoints);
		~AreaLight() = default;

		Vec3 sideA;
		Vec3 sideB;
		Vec3 cornerPos;

		/// <summary>
		/// The squared number of points in light to test for shadows if not using antialiasing
		/// </summary>
		int sqrdNbPoints = 3;


		float GetIntensityAtPoint(const Vec3& EmissionPoint, const std::vector<Object*>& objectsInScene, AntialiasingMode antialiasingMode);

	};

}