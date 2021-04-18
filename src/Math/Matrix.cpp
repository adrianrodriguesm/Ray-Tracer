#include "pch.h"
#include "Math/Matrix.h"
#include "Core/Constant.h"
namespace rayTracer
{
	/////////////////////////////////////////////////////////////////////// Mat4
	Mat4::Mat4()
	{
		for (int i = 0; i < 16; i++)
		{
			this->data[i] = 0.0f;
		}
		this->data[0] = this->data[1 + 1 * 4] = this->data[2 + 2 * 4] = this->data[3 + 3 * 4] = 1.0f;
	}

	Mat4::Mat4(const float k)
	{
		for (int i = 0; i < 16; i++)
		{
			this->data[i] = 0.f;
		}
		this->data[0] = this->data[1 + 1 * 4] = this->data[2 + 2 * 4] = this->data[3 + 3 * 4] = k;
	}

	Mat4::Mat4(const float m0, const float m4, const float m8, const float m12,
		const float m1, const float m5, const float m9, const float m13,
		const float m2, const float m6, const float m10, const float m14,
		const float m3, const float m7, const float m11, const float m15)
	{
		// 4x4 matrix - column major
		//    0    4    8    12
		//    1    5    9    13
		//    2    6    10   14
		//    3    7    11   15
		this->data[0] = m0;
		this->data[1] = m1;
		this->data[2] = m2;
		this->data[3] = m3;

		this->data[4] = m4;
		this->data[5] = m5;
		this->data[6] = m6;
		this->data[7] = m7;

		this->data[8] = m8;
		this->data[9] = m9;
		this->data[10] = m10;
		this->data[11] = m11;

		this->data[12] = m12;
		this->data[13] = m13;
		this->data[14] = m14;
		this->data[15] = m15;

	}

	Mat4::Mat4(const float matrixData[])
	{
		std::memcpy(data, matrixData, 16 * sizeof(float));
	}

	Mat4::Mat4(const Mat4& other)
	{
		std::memcpy(data, other.data, 16 * sizeof(float));
	}

	Mat4::Mat4(Mat4&& other) noexcept
	{
		std::memmove(data, other.data, 16 * sizeof(float));
	}

	float* Mat4::Data()
	{
		return data;
	}

	const float* Mat4::Data() const
	{
		return data;
	}



	Mat4& Mat4::operator=(const Mat4& other)
	{
		std::memcpy(data, other.data, 16 * sizeof(float));
		return *this;
	}

	Mat4& Mat4::operator=(Mat4&& other) noexcept
	{
		std::memmove(data, other.data, 16 * sizeof(float));
		return *this;
	}

	const void Clean(Mat4& matrix)
	{
		for (int i = 0; i < 16; i++) {
			if (fabs(matrix.data[i]) < EPSILON) matrix.data[i] = 0.0f;
		}
	}

	const Mat4 Transpose(const Mat4& matrix)
	{
		Mat4 transpose;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				transpose.data[i + j * 4] = matrix.data[j + i * 4];
			}
		}
		return transpose;
	}

	const float Determinant(const Mat4& m)
	{
		float inv[16], det;

		inv[0] = m.data[5] * m.data[10] * m.data[15] -
			m.data[5] * m.data[11] * m.data[14] -
			m.data[9] * m.data[6] * m.data[15] +
			m.data[9] * m.data[7] * m.data[14] +
			m.data[13] * m.data[6] * m.data[11] -
			m.data[13] * m.data[7] * m.data[10];

		inv[4] = -m.data[4] * m.data[10] * m.data[15] +
			m.data[4] * m.data[11] * m.data[14] +
			m.data[8] * m.data[6] * m.data[15] -
			m.data[8] * m.data[7] * m.data[14] -
			m.data[12] * m.data[6] * m.data[11] +
			m.data[12] * m.data[7] * m.data[10];

		inv[8] = m.data[4] * m.data[9] * m.data[15] -
			m.data[4] * m.data[11] * m.data[13] -
			m.data[8] * m.data[5] * m.data[15] +
			m.data[8] * m.data[7] * m.data[13] +
			m.data[12] * m.data[5] * m.data[11] -
			m.data[12] * m.data[7] * m.data[9];

		inv[12] = -m.data[4] * m.data[9] * m.data[14] +
			m.data[4] * m.data[10] * m.data[13] +
			m.data[8] * m.data[5] * m.data[14] -
			m.data[8] * m.data[6] * m.data[13] -
			m.data[12] * m.data[5] * m.data[10] +
			m.data[12] * m.data[6] * m.data[9];

		inv[1] = -m.data[1] * m.data[10] * m.data[15] +
			m.data[1] * m.data[11] * m.data[14] +
			m.data[9] * m.data[2] * m.data[15] -
			m.data[9] * m.data[3] * m.data[14] -
			m.data[13] * m.data[2] * m.data[11] +
			m.data[13] * m.data[3] * m.data[10];

		inv[5] = m.data[0] * m.data[10] * m.data[15] -
			m.data[0] * m.data[11] * m.data[14] -
			m.data[8] * m.data[2] * m.data[15] +
			m.data[8] * m.data[3] * m.data[14] +
			m.data[12] * m.data[2] * m.data[11] -
			m.data[12] * m.data[3] * m.data[10];

		inv[9] = -m.data[0] * m.data[9] * m.data[15] +
			m.data[0] * m.data[11] * m.data[13] +
			m.data[8] * m.data[1] * m.data[15] -
			m.data[8] * m.data[3] * m.data[13] -
			m.data[12] * m.data[1] * m.data[11] +
			m.data[12] * m.data[3] * m.data[9];

		inv[13] = m.data[0] * m.data[9] * m.data[14] -
			m.data[0] * m.data[10] * m.data[13] -
			m.data[8] * m.data[1] * m.data[14] +
			m.data[8] * m.data[2] * m.data[13] +
			m.data[12] * m.data[1] * m.data[10] -
			m.data[12] * m.data[2] * m.data[9];

		inv[2] = m.data[1] * m.data[6] * m.data[15] -
			m.data[1] * m.data[7] * m.data[14] -
			m.data[5] * m.data[2] * m.data[15] +
			m.data[5] * m.data[3] * m.data[14] +
			m.data[13] * m.data[2] * m.data[7] -
			m.data[13] * m.data[3] * m.data[6];

		inv[6] = -m.data[0] * m.data[6] * m.data[15] +
			m.data[0] * m.data[7] * m.data[14] +
			m.data[4] * m.data[2] * m.data[15] -
			m.data[4] * m.data[3] * m.data[14] -
			m.data[12] * m.data[2] * m.data[7] +
			m.data[12] * m.data[3] * m.data[6];

		inv[10] = m.data[0] * m.data[5] * m.data[15] -
			m.data[0] * m.data[7] * m.data[13] -
			m.data[4] * m.data[1] * m.data[15] +
			m.data[4] * m.data[3] * m.data[13] +
			m.data[12] * m.data[1] * m.data[7] -
			m.data[12] * m.data[3] * m.data[5];

		inv[14] = -m.data[0] * m.data[5] * m.data[14] +
			m.data[0] * m.data[6] * m.data[13] +
			m.data[4] * m.data[1] * m.data[14] -
			m.data[4] * m.data[2] * m.data[13] -
			m.data[12] * m.data[1] * m.data[6] +
			m.data[12] * m.data[2] * m.data[5];

		inv[3] = -m.data[1] * m.data[6] * m.data[11] +
			m.data[1] * m.data[7] * m.data[10] +
			m.data[5] * m.data[2] * m.data[11] -
			m.data[5] * m.data[3] * m.data[10] -
			m.data[9] * m.data[2] * m.data[7] +
			m.data[9] * m.data[3] * m.data[6];

		inv[7] = m.data[0] * m.data[6] * m.data[11] -
			m.data[0] * m.data[7] * m.data[10] -
			m.data[4] * m.data[2] * m.data[11] +
			m.data[4] * m.data[3] * m.data[10] +
			m.data[8] * m.data[2] * m.data[7] -
			m.data[8] * m.data[3] * m.data[6];

		inv[11] = -m.data[0] * m.data[5] * m.data[11] +
			m.data[0] * m.data[7] * m.data[9] +
			m.data[4] * m.data[1] * m.data[11] -
			m.data[4] * m.data[3] * m.data[9] -
			m.data[8] * m.data[1] * m.data[7] +
			m.data[8] * m.data[3] * m.data[5];

		inv[15] = m.data[0] * m.data[5] * m.data[10] -
			m.data[0] * m.data[6] * m.data[9] -
			m.data[4] * m.data[1] * m.data[10] +
			m.data[4] * m.data[2] * m.data[9] +
			m.data[8] * m.data[1] * m.data[6] -
			m.data[8] * m.data[2] * m.data[5];

		det = m.data[0] * inv[0] + m.data[1] * inv[4] + m.data[2] * inv[8] + m.data[3] * inv[12];

		return det;
	}

	const Mat4 operator+(const Mat4& m0, const Mat4& m1)
	{
		Mat4 result;
		for (int i = 0; i < 16; i++) {
			result.data[i] = m0.data[i] + m1.data[i];
		}
		return result;
	}

	const Mat4 operator-(const Mat4& m0, const Mat4& m1)
	{
		Mat4 result;
		for (int i = 0; i < 16; i++) {
			result.data[i] = m0.data[i] - m1.data[i];
		}
		return result;
	}

	const Mat4 operator*(const float k, const Mat4& m)
	{
		Mat4 result;
		for (int i = 0; i < 16; i++) {
			result.data[i] = m.data[i] * k;
		}
		return result;
	}

	const Mat4 operator*(const Mat4& m, const float k)
	{
		Mat4 result;
		for (int i = 0; i < 16; i++) {
			result.data[i] = m.data[i] * k;
		}
		return result;
	}

	const Mat4 operator*(const Mat4& m0, const Mat4& m1)
	{
		Mat4 result;
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				float sum = 0.0f;
				for (int e = 0; e < 4; e++)
				{
					sum += m0.data[x + e * 4] * m1.data[e + y * 4];
				}
				result.data[x + y * 4] = sum;
			}
		}
		return result;
	}

	const bool operator==(const Mat4& m0, const Mat4& m1)
	{
		for (int i = 0; i < 16; i++) {

			if (fabs(m0.data[i] - m1.data[i]) > EPSILON)
			{
				return false;
			}
		}
		return true;
	}

	const bool operator!=(const Mat4& m0, const Mat4& m1)
	{
		return !(m0 == m1);
	}

	std::ostream& operator<<(std::ostream& os, const Mat4& m)
	{
		os << "4D Matrix: " << std::endl;
		os << "| " << m.data[0 + 0 * 4] << "    " << m.data[0 + 1 * 4] << "    " << m.data[0 + 2 * 4] << "    " << m.data[0 + 3 * 4] << " |" << std::endl;
		os << "| " << m.data[1 + 0 * 4] << "    " << m.data[1 + 1 * 4] << "    " << m.data[1 + 2 * 4] << "    " << m.data[1 + 3 * 4] << " |" << std::endl;
		os << "| " << m.data[2 + 0 * 4] << "    " << m.data[2 + 1 * 4] << "    " << m.data[2 + 2 * 4] << "    " << m.data[2 + 3 * 4] << " |" << std::endl;
		os << "| " << m.data[3 + 0 * 4] << "    " << m.data[3 + 1 * 4] << "    " << m.data[3 + 2 * 4] << "    " << m.data[3 + 3 * 4] << " |" << std::endl;
		return os;
	}

	const Mat4 MatFactory::CreateZeroMat4()
	{
		return Mat4(0.f);
	}
	const Mat4 MatFactory::CreateIdentityMat4()
	{
		return Mat4();
	}

	const Mat4 MatFactory::CreateScaleMat4(const Vec3& v)
	{
		Mat4 scaleMatrix = Mat4();
		scaleMatrix.data[0] = v.x;
		scaleMatrix.data[5] = v.y;
		scaleMatrix.data[10] = v.z;

		return scaleMatrix;
	}

	const Mat4 MatFactory::CreateTranslationMat4(const Vec3& v)
	{
		Mat4 translationMatrix = Mat4();
		translationMatrix.data[12] = v.x;
		translationMatrix.data[13] = v.y;
		translationMatrix.data[14] = v.z;

		return translationMatrix;
	}


	const Mat4 MatFactory::CreateLookAtMatrix(const Vec3& eye, const Vec3& center, const Vec3& up)
	{
		Vec3 e = eye;
		Vec3 v = Normalize(center - e);
		Vec3 s = Normalize(v.CrossProduct(up));
		Vec3 u = s.CrossProduct(v);
		return Mat4(s.x, s.y, s.z, -(e.DotProduct(s)),
			u.x, u.y, u.z, -(e.DotProduct(u)),
			-v.x, -v.y, -v.z, e.DotProduct(v),
			0.f, 0.f, 0.f, 1.f);
	}

	const Mat4 MatFactory::CreateOrthographicProjectionMatrix(const float left, const float right, const float bottom, const float top, const float nearPlane, const float farPlane)
	{

		return Mat4(2.f / (right - left), 0.f, 0.f, (left + right) / (left - right),
			0.f, 2.f / (top - bottom), 0.f, (bottom + top) / (bottom - top),
			0.f, 0.f, 2.f / (nearPlane - farPlane), (nearPlane + farPlane) / (nearPlane - farPlane),
			0.f, 0.f, 0.f, 1.f);
	}
	const Mat4 MatFactory::CreatePerspectiveProjectionMatrix(const float fovy, const float ratio, const float nearPlane, const float farPlane)
	{

		float d = 1 / tan(fovy / 2);

		return Mat4(d / ratio, 0.f, 0.f, 0.f,
			0.f, d, 0.f, 0.f,
			0.f, 0.f, (nearPlane + farPlane) / (nearPlane - farPlane), (2 * nearPlane * farPlane) / (nearPlane - farPlane),
			0.f, 0.f, -1.f, 0.f);
	}
}