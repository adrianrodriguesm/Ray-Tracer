#pragma once

namespace rayTracer
{		
	/////////////////////////////////////////////////////////////////////// Constants
	const float M_PI = 3.14159265358979323846f;
	const float EPSILON = (float)1.0e-5;
	const float DEGREES_TO_RADIANS = 0.01745329251994329547f;
	const float RADIANS_TO_DEGREES = 57.29577951308232185913f;
	const float DISPLACEMENT_BIAS = 1.E-4; // For reflecting and refraction to avoid self occlusion

	namespace Vector
	{
		const Vec2 AXIS2D_X = { 1.0f, 0.0f };
		const Vec2 AXIS2D_Y = { 1.0f, 1.0f };

		const Vec3 AXIS3D_X = { 1.0f, 0.0f, 0.0f };
		const Vec3 AXIS3D_Y = { 0.0f, 1.0f, 0.0f };
		const Vec3 AXIS3D_Z = { 0.0f, 0.0f, 1.0f };

		const Vec3 FORWARD = { 0.0f,  0.0f, -1.0f };
		const Vec3 BACK = { 0.0f,  0.0f,  1.0f };
		const Vec3 UP = { 0.0f,  1.0f,  0.0f };
		const Vec3 DOWN = { 0.0f, -1.0f,  0.0f };
		const Vec3 RIGHT = { 1.0f,  0.0f,  0.0f };
		const Vec3 LEFT = { -1.0f,  0.0f,  0.0f };
	}

	namespace Color
	{
		const Vec3 RED =	{ 1.0f, 0.0f, 0.0f };
		const Vec3 BLUE =	{ 0.0f, 1.0f, 0.0f };
		const Vec3 GREEN =	{ 0.0f, 0.0f, 1.0f };
		const Vec3 YELLOW = { 1.0f, 0.0f, 1.0f };
		const Vec3 MAGENTA = { 1.0f, 1.0f, 0.0f };
		const Vec3 CYAN =	{ 0.0f, 1.0f, 1.0f };
		const Vec3 BLACK = { 0.0f, 0.0f, 0.0f };
		const Vec3 WHITE = { 1.0f, 1.0f, 1.0f };
	}
}