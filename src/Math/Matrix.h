#pragma once
#include "Math/Vector.h"
namespace rayTracer
{
	struct Mat4
	{
		float data[16];

		Mat4();
		Mat4(const float k);
		Mat4(const float m0, const float m4, const float m8, const float m12,
			const float m1, const float m5, const float m9, const float m13,
			const float m2, const float m6, const float m10, const float m14,
			const float m3, const float m7, const float m11, const float m15);
		Mat4(const float matrixData[]);
		Mat4(const Mat4& other);
		Mat4(Mat4&& other) noexcept;

		float* Data();
		const float* Data() const;
		friend const void Clean(Mat4& matrix);
		friend const Mat4 Transpose(const Mat4& matrix);
		friend const float Determinant(const Mat4& matrix);

		Mat4& operator = (const Mat4& m);
		Mat4& operator = (Mat4&& m) noexcept;

		friend const Mat4 operator + (const Mat4& m0, const Mat4& m1);
		friend const Mat4 operator - (const Mat4& m0, const Mat4& m1);
		friend const Mat4 operator * (const float k, const Mat4& m);
		friend const Mat4 operator * (const Mat4& m, const float k);
		//friend const Vec4 operator * (const Mat4& m, const Vec4& v);
		friend const Mat4 operator * (const Mat4& m0, const Mat4& m1);
		friend const bool operator == (const Mat4& m0, const Mat4& m1);
		friend const bool operator != (const Mat4& m0, const Mat4& m1);
		friend std::ostream& operator << (std::ostream& os, const Mat4& m);
	};

	struct MatFactory
	{
		static const Mat4 CreateZeroMat4();
		static const Mat4 CreateIdentityMat4();
		static const Mat4 CreateScaleMat4(const Vec3& v);
		// Work in radians
		//static const Mat4 CreateRotationMat4(const float angle, const Vec3& v);
		static const Mat4 CreateTranslationMat4(const Vec3& v);
		static const Mat4 CreateLookAtMatrix(const Vec3& eye, const Vec3& center, const Vec3& up);
		static const Mat4 CreateOrthographicProjectionMatrix(const float left, const float right, const float bottom, const float top, const float nearPlane, const float farPlane);
		// Work in radians
		static const Mat4 CreatePerspectiveProjectionMatrix(const float fovy, const float ratio, const float nearPlane, const float farPlane);
	};
}