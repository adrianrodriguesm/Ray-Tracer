#include "pch.h"
#include "AntiAliasing.h"
#include "Math/Random.h"
#include "Renderer/Sample.h"
namespace rayTracer
{
	/// <summary>
	/// Returns a single sampling point in the center of the pixel.
	/// </summary>
	/// <param name="pixelCoords"></param>
	/// <param name="nbSamples"></param>
	/// <returns></returns>
	std::vector<Vec2> SingularSampling::GetSamplingOffsets(int nbSamples)
	{
		std::vector<Vec2> sampleOffsets = std::vector<Vec2>();
		sampleOffsets.push_back(Vec2(0.5f, 0.5f));
		return sampleOffsets;
	}

	std::vector<Vec2> RegularSampling::GetSamplingOffsets(int nbsampleOffsets)
	{
		std::vector<Vec2> sampleOffsets = std::vector<Vec2>();
		for (uint32_t i = 0; i < nbsampleOffsets; i++)
		{
			for (uint32_t j = 0; j < nbsampleOffsets; j++)
			{
				Vec2 sampleOffset;  //viewport coordinates
				sampleOffset.x = (i + 0.5f) / nbsampleOffsets;
				sampleOffset.y = (j + 0.5f) / nbsampleOffsets;
				sampleOffsets.push_back(sampleOffset);
			}
		}
		return sampleOffsets;
	}

	std::vector<Vec2> JitteringSampling::GetSamplingOffsets(int nbsampleOffsets)
	{
		std::vector<Vec2> sampleOffsets = std::vector<Vec2>();
		for (uint32_t i = 0; i < nbsampleOffsets; i++)
		{
			for (uint32_t j = 0; j < nbsampleOffsets; j++)
			{
				Vec2 sampleOffset;  //viewport coordinates
				sampleOffset.x = (i + Random::Float()) / nbsampleOffsets;
				sampleOffset.y = (j + Random::Float()) / nbsampleOffsets;
				sampleOffsets.push_back(sampleOffset);
			}
		}
		return sampleOffsets;
	}
	std::shared_ptr<SampleAPI> SampleAPI::Create(AntialiasingMode mode)
	{
		switch (mode)
		{
			case AntialiasingMode::NONE:			 return std::make_shared<SingularSampling>();
			case AntialiasingMode::REGULAR_SAMPLING: return std::make_shared<RegularSampling>();  
			case AntialiasingMode::JITTERING:		 return std::make_shared<JitteringSampling>();  
			default: ASSERT(false, "ERROR: The attempted antialasing mode has not been implemented");
		};
		return nullptr;
	}
}
