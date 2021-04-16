#pragma once
#include "Math/Vector.h"
namespace rayTracer::utils
{
	inline Vec3 ApplyToneMapping(const Vec3& color, const  float exposure)
	{
		// Reinhard tonemapping
		static constexpr float pureWhite = 1.f;
		float luminance = DotProduct(color, Vec3(0.2126f, 0.7152f, 0.0722f));
		float mappedLuminance = (luminance * (1 + luminance / (pureWhite * pureWhite))) / (1 + luminance);

		Vec3 hdrColor = (mappedLuminance / luminance) * color * exposure;

		return hdrColor;
	}
	inline Vec3 ConvertColorFromLinearToGammaSpace(Vec3& color)
	{
		static constexpr float gamma = 2.2f;
		Vec3 gammaColor = { pow(color.r, 1 / gamma), pow(color.g, 1 / gamma), pow(color.b, 1 / gamma) };
		return gammaColor;
	}
}