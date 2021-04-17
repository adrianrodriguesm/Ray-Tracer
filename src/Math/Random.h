#pragma once
#include <random>
namespace rayTracer
{
	class Random
	{
	public:
		static void Init()
		{
			s_RandomEngine.seed(std::random_device()());
		}

		// Return a random between 0 and 1
		static float Float()
		{
			return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
		}

		/// <summary>
		/// Returns a random float between min and max.
		/// </summary>
		/// <param name="min"></param>
		/// <param name="max"></param>
		/// <returns></returns>
		static float Float(float min, float max)
		{
			float zero_one = (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
			return min + max * zero_one;
		}

	private:
		static std::mt19937 s_RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
	};
	
}