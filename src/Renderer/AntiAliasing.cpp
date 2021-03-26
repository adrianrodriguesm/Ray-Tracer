#include "AntiAliasing.h"
namespace rayTracer
{
	/// <summary>
	/// Returns a single sampling point in the center of the pixel.
	/// </summary>
	/// <param name="pixelCoords"></param>
	/// <param name="nbSamples"></param>
	/// <returns></returns>
	std::vector<Vec2> SingularSampling::GetSamplingPoints(const Vec2& pixelCoords, int nbSamples)
	{
		std::vector<Vec2> samples = std::vector<Vec2>();
		samples.push_back(pixelCoords + Vec2(0.5f, 0.5f));
		return samples;
	}

	std::vector<Vec2> RegularSampling::GetSamplingPoints(const Vec2& pixelCoords, int nbSamples)
	{
		std::cout << "\nERROR: The attempted sampling method has not been implemented\n";
		exit(EXIT_FAILURE);
	}

	std::vector<Vec2> JitteringSampling::GetSamplingPoints(const Vec2& pixelCoords, int nbSamples)
	{
		std::vector<Vec2> samples = std::vector<Vec2>();
		for (uint32_t i = 0; i < nbSamples; i++)
		{
			for (uint32_t j = 0; j < nbSamples; j++)
			{
				Vec2 sample;  //viewport coordinates
				sample.x = pixelCoords.x + (i + Random::Float()) / nbSamples;
				sample.y = pixelCoords.y + (j + Random::Float()) / nbSamples;
				samples.push_back(sample);
			}
		}
		return samples;
	}
}
