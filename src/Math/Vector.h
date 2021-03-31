#pragma once
#include <iostream>
namespace rayTracer
{
	struct Vec2;
	struct Vec3;
	struct Vec3Int;
	/////////////////////////////////////////////////////////////////////// Vec2
	struct Vec2
	{
		union
		{
			struct { float x, y; };
			float components[2] = { 0, 0 };
		};

		Vec2();
		Vec2(const float& k);
		Vec2(const float& x, const float& y);
		Vec2(const Vec2& other);
		Vec2(Vec2&& other) noexcept;

		float* Data();
		const float* Data() const;
		float SqrMagnitude() const;
		friend float Magnitude(const Vec2& vector);
		friend Vec2 Normalize(const Vec2& vector);
		friend Vec2 Lerp(const Vec2& v0, const Vec2& v1, const float time);

		float DotProduct(const Vec2& other) const;

		friend Vec2 operator +  (const Vec2& left, const Vec2& right);
		friend Vec2 operator -  (const Vec2& left, const Vec2& right);
		friend Vec2 operator *  (const Vec2& vector, const float scalar);
		friend Vec2 operator *  (const float scalar, const Vec2& Vector);
		friend Vec2 operator /  (const Vec2& left, const float right);
		friend bool operator == (const Vec2& left, const Vec2& right);
		friend bool operator != (const Vec2& left, const Vec2& right);
		friend bool operator < (const Vec2& left, const Vec2& right);
		friend bool operator > (const Vec2& left, const Vec2& right);
		Vec2 operator-() const;
		Vec2& operator += (const Vec2& other);
		Vec2& operator -= (const Vec2& other);
		Vec2& operator *= (const float& scalar);
		Vec2& operator = (const Vec2& other);
		Vec2& operator = (Vec2&& other) noexcept;

		friend std::ostream& operator << (std::ostream& stream, const Vec2& vector);
		friend std::istream& operator >> (std::istream& stream, Vec2& vector);
	};
	/////////////////////////////////////////////////////////////////////// Vec3
	struct Vec3
	{
		union
		{
			struct { float x, y, z; };
			struct { float r, g, b; };
			float components[3] = { 0, 0, 0 };
		};

		Vec3();
		Vec3(const float& k);
		Vec3(const float& x, const float& y, const float& z);
		Vec3(const Vec3& other);
		Vec3(Vec3&& other) noexcept;

		Vec2 ToVec2();

		float* Data();
		const float* Data() const;
		friend const void Clean(Vec3& v);
		float SqrMagnitude() const;
		float Magnitude() const;
		friend float Magnitude(const Vec3& v);
		friend Vec3 Lerp(const Vec3& v0, const Vec3& v1, const float time);
		// Vectors atribute need to be normalize
		friend Vec3 Reflect(const Vec3& vector, const Vec3& normal);
		// Vectors atribute need to be normalize
		friend Vec3 Refract(const Vec3& vector, const Vec3& normal, const float refractionIndex);
		// Returns this vector normalized
		Vec3& Normalized();
		// Return a copy of the vector normalized
		friend Vec3 Normalize(const Vec3& vector);
		// Scalar and Vectorial product
		float DotProduct(const Vec3& other) const;
		friend float DotProduct(const Vec3& v0, const Vec3& v1);
		Vec3 CrossProduct(const Vec3& other) const;
		friend Vec3 CrossProduct(const Vec3& v0, const Vec3& v1);
		// Vector need to be normalize
		friend Vec3 ProjectOnPlane(const Vec3& vectorDir, const Vec3& normal);
		friend float Distance(const Vec3& v0, const Vec3& v1);

		friend const Vec3 operator + (const Vec3& left, const Vec3& right);
		
		friend const Vec3 operator - (const Vec3& left, const Vec3& right);
		friend const Vec3 operator * (const Vec3& Vector, const float scalar);
		friend const Vec3 operator * (const float scalar, const Vec3& Vector);
		friend const Vec3 operator * (const Vec3& v1, const Vec3& v2);
		friend const Vec3 operator / (const Vec3& left, const float right);
		friend const Vec3 operator / (const Vec3& left, const Vec3& right);
		friend const Vec3 operator / (const Vec3& left, const Vec3Int& right);
		friend const bool operator == (const Vec3& left, const Vec3& right);
		friend const bool operator != (const Vec3& left, const Vec3& right);

		Vec3   operator -   () const;
		Vec3&  operator += (const Vec3& other);
		Vec3&  operator -= (const Vec3& other);
		Vec3&  operator *= (const float& scalar);
		Vec3&  operator =  (const Vec3& other);
		Vec3&  operator =  (Vec3&& other) noexcept;
		float  operator [] (const uint32_t index) const;
		float& operator [] (uint32_t index);

		friend std::ostream& operator << (std::ostream& stream, const Vec3& Vector);
		friend std::istream& operator >> (std::istream& stream, Vec3& Vector);

		// TODO: CHECK IF NECESARY
		Vec3& ClampColor();
		friend Vec3 sample_unit_disk();
	};
	
	/////////////////////////////////////////////////////////////////////// Vec3
	struct Vec3Int
	{
		union
		{
			struct { int x, y, z; };
			struct { int r, g, b; };
			int components[3] = { 0, 0, 0 };
		};

		Vec3Int();
		Vec3Int(const int& k);
		Vec3Int(const int& x, const int& y, const int& z);
		Vec3Int(const Vec3Int& other);
		Vec3Int(Vec3Int&& other) noexcept;

		int* Data();
		const int* Data() const;

		friend const Vec3Int operator + (const Vec3Int& left, const Vec3Int& right);

		friend const Vec3Int operator - (const Vec3Int& left, const Vec3Int& right);
		friend const Vec3Int operator * (const Vec3Int& Vector, const int scalar);
		friend const Vec3Int operator * (const int scalar, const Vec3Int& Vector);
		friend const Vec3Int operator * (const Vec3Int& v1, const Vec3Int& v2);
		friend const Vec3Int operator / (const Vec3Int& left, const int right);
		friend const Vec3Int operator / (const Vec3Int& left, const Vec3Int& right);
		friend const bool operator == (const Vec3Int& left, const Vec3Int& right);
		friend const bool operator != (const Vec3Int& left, const Vec3Int& right);

		Vec3Int   operator -   () const;
		Vec3Int& operator += (const Vec3Int& other);
		Vec3Int& operator -= (const Vec3Int& other);
		Vec3Int& operator *= (const int& scalar);
		Vec3Int& operator =  (const Vec3Int& other);
		Vec3Int& operator =  (Vec3Int&& other) noexcept;
		int  operator [] (const uint32_t index) const;
		int& operator [] (uint32_t index);

		friend std::ostream& operator << (std::ostream& stream, const Vec3Int& Vector);
		friend std::istream& operator >> (std::istream& stream, Vec3Int& Vector);

	};
}
