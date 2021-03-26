#pragma once
#include "Math/Vector.h"
#include "Math/Random.h"
#include <vector>

namespace rayTracer
{
	enum struct AntialiasingMode 
	{
		NONE,
		REGULAR_SAMPLING,
		JITTERING
	};

	class SingularSampling
	{
	public:
		static std::vector<Vec2> GetSamplingPoints(const Vec2& pixelCoords, int samples);
	};

	class RegularSampling
	{
	public:
		static std::vector<Vec2> GetSamplingPoints(const Vec2& pixelCoords, int samples);
	};

	class JitteringSampling
	{
	public:
		static std::vector<Vec2> GetSamplingPoints(const Vec2& pixelCoords, int samples);
	};
}

