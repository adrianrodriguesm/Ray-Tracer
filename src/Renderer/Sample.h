#pragma once
#include "Math/Vector.h"
#include <vector>
#include <memory>
#include <Core/Constant.h>
#include <Core/Base.h>
#include "Renderer/AntiAliasing.h"
namespace rayTracer
{
	enum class AntialiasingMode
	{
		NONE, REGULAR_SAMPLING, JITTERING
	};
	class Sample
	{
	public:
		// With exp = 0 with obtain a uniform sampling
		inline static std::vector<Vec3> MapSamplesToHemisphere(const std::vector<Vec2>& samples, float exp = 0)
		{
			int size = samples.size();
			std::vector<Vec3> hemisphereSamples;
			hemisphereSamples.reserve(samples.size());
			for (int j = 0; j < size; j++)
			{
				// Sample to hemisphere using a cosine power distribution
				// https://blog.thomaspoulet.fr/uniform-sampling-on-unit-hemisphere/
				float phi = 2.0f * M_PI * samples[j].x;
				float cosPhi = cosf(phi);
				float sinPhi = sinf(phi);
				float cosTheta = powf((1.0f * samples[j].y), 1.0f / (exp + 1.0f));
				float sintheta = sqrtf(1.0f - cosTheta * cosTheta);
				hemisphereSamples.push_back({ sintheta * cosPhi, sintheta * cosPhi, cosTheta });
			}
			return hemisphereSamples;
		}
		inline static void SwichAntialiasingMode(AntialiasingMode mode)
		{
			s_Sampler = SampleAPI::Create(mode);
		}
		inline static std::vector<Vec2> GetSamplingOffsets(uint32_t nbSamples)
		{
			return s_Sampler->GetSamplingOffsets(nbSamples);
		}
	private:
		static std::shared_ptr<SampleAPI> s_Sampler;
	};
}