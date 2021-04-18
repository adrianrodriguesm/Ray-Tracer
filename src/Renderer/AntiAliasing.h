#pragma once
#include "Math/Vector.h"
#include <vector>
namespace rayTracer
{
	enum class AntialiasingMode;
	class SampleAPI
	{
	public:
		SampleAPI() = default;
		virtual ~SampleAPI() = default;
		virtual std::vector<Vec2> GetSamplingOffsets(int nbSamples) = 0;
		static std::shared_ptr<SampleAPI> Create(AntialiasingMode mode);
	};

	class SingularSampling : public SampleAPI
	{
	public:
		SingularSampling() = default;
		virtual ~SingularSampling() = default;
		virtual std::vector<Vec2> GetSamplingOffsets(int nbSamples) override;
	};

	class RegularSampling : public SampleAPI
	{
	public:
		RegularSampling() = default;
		virtual ~RegularSampling() = default;
		virtual std::vector<Vec2> GetSamplingOffsets(int nbSamples) override;
	};

	class JitteringSampling : public SampleAPI
	{
	public:
		JitteringSampling() = default;
		virtual ~JitteringSampling() = default;
		virtual std::vector<Vec2> GetSamplingOffsets(int nbSamples) override;
	};
}

