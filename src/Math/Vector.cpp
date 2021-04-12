#include "Vector.h"
#include "Core/Constant.h"
#include "Core/Base.h"
#include "Math/Maths.h"
#include "Math/Random.h"
#include <algorithm>
namespace rayTracer
{
	/////////////////////////////////////////////////////////////////////// Vec2
#pragma region Vec2
	Vec2::Vec2()
		: x(0), y(0)
	{
	}

	Vec2::Vec2(const float& k)
		: x(k), y(k)
	{
	}

	Vec2::Vec2(const float& x, const float& y)
		: x(x), y(y)
	{
	}

	Vec2::Vec2(const Vec2& other)
		: x(other.x), y(other.y)
	{
	}

	Vec2::Vec2(Vec2&& other) noexcept
		: x(other.x), y(other.y)
	{
	}



	float* Vec2::Data()
	{
		return &x;
	}

	const float* Vec2::Data() const
	{
		return &x;
	}

	float Vec2::SqrMagnitude() const
	{
		float x = powf(this->x, 2);
		float y = powf(this->y, 2);
		return x + y;
	}
	float Magnitude(const Vec2& vector)
	{
		float x = powf(vector.x, 2);
		float y = powf(vector.y, 2);
		return sqrtf(x + y);
	}

	Vec2 Normalize(const Vec2& vector)
	{
		float mag = Magnitude(vector);
		return Vec2(vector.x / mag, vector.y / mag);
	}

	float Vec2::DotProduct(const Vec2& other) const
	{
		return (this->x * other.x + this->y * other.y);
	}

	Vec2 Lerp(const Vec2& v0, const Vec2& v1, const float time)
	{
		return v0 + ((v1 - v0) * time);
	}

	Vec2 operator+(const Vec2& left, const Vec2& right)
	{
		return Vec2(left.x + right.x, left.y + right.y);
	}

	Vec2 operator-(const Vec2& left, const Vec2& right)
	{
		return Vec2(left.x - right.x, left.y - right.y);
	}

	Vec2 operator*(const Vec2& vector, const float scalar)
	{
		return Vec2(vector.x * scalar, vector.y * scalar);
	}

	Vec2 operator*(const float scalar, const Vec2& vector)
	{
		return Vec2(vector.x * scalar, vector.y * scalar);
	}

	Vec2 operator/(const Vec2& Vector, const float scalar)
	{
		return Vec2(Vector.x / scalar, Vector.y / scalar);
	}

	bool operator==(const Vec2& left, const Vec2& right)
	{

		return fabs(left.x - right.x) <= EPSILON && fabs(left.y - right.y) <= EPSILON;
	}

	bool operator!=(const Vec2& left, const Vec2& right)
	{
		return !(left == right);
	}

	bool operator<(const Vec2& left, const Vec2& right)
	{
		return left.x < right.x && left.y < right.y;
	}

	bool operator>(const Vec2& left, const Vec2& right)
	{
		return left.x > right.x && left.y > right.y;
	}

	Vec2& Vec2::operator=(const Vec2& other)
	{
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

	Vec2& Vec2::operator=(Vec2&& other) noexcept
	{
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

	Vec2 Vec2::operator-() const
	{
		return -1.f * (*this);
	}

	Vec2& Vec2::operator+=(const Vec2& other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}

	Vec2& Vec2::operator-=(const Vec2& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}

	Vec2& Vec2::operator*=(const float& scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		return *this;
	}

	std::ostream& operator<<(std::ostream& stream, const Vec2& Vector)
	{
		stream << "Vector 2D : (" << Vector.x << ", " << Vector.y << ")";
		return stream;
	}

	std::istream& operator>>(std::istream& stream, Vec2& Vector)
	{
		std::cout << "Type x coordenate: ";
		stream >> Vector.x;
		std::cout << "Type y coordenate: ";
		stream >> Vector.y;
		return stream;
	}
#pragma endregion Vec2
	/////////////////////////////////////////////////////////////////////// Vec3
#pragma region Vec3
	Vec3::Vec3()
		: x(0), y(0), z(0)
	{}

	Vec3::Vec3(const float& k)
		: x(k), y(k), z(k)
	{}

	Vec3::Vec3(const float& x, const float& y, const float& z)
		: x(x), y(y), z(z)
	{}

	Vec3::Vec3(const Vec3& other)
		: x(other.x), y(other.y), z(other.z)
	{
	}

	Vec3::Vec3(Vec3&& other) noexcept
		: x(other.x), y(other.y), z(other.z)
	{
	}

	Vec2 Vec3::ToVec2()
	{
		return Vec2(this->x, this->y);
	}


	float Vec3::GetAxisValue(int axis) {
		return (axis == 0) ? x : (axis == 1) ? y : z;
	}

	float* Vec3::Data()
	{
		return &x;
	}

	const float* Vec3::Data() const
	{
		return &x;
	}

	const void Clean(Vec3& vector)
	{
		if (fabs(vector.x) < EPSILON) vector.x = 0.0f;
		if (fabs(vector.y) < EPSILON) vector.y = 0.0f;
		if (fabs(vector.z) < EPSILON) vector.z = 0.0f;
	}

	float Vec3::SqrMagnitude() const
	{
		float x = powf(this->x, 2);
		float y = powf(this->y, 2);
		float z = powf(this->z, 2);
		return (x + y + z);
	}

	float Vec3::Magnitude() const
	{
		float x = powf(this->x, 2);
		float y = powf(this->y, 2);
		float z = powf(this->z, 2);
		return sqrtf(x + y + z);
	}

	float Magnitude(const Vec3& vector)
	{
		float x = powf(vector.x, 2);
		float y = powf(vector.y, 2);
		float z = powf(vector.z, 2);
		return sqrtf(x + y + z);
	}

	Vec3 Normalize(const Vec3& vector)
	{
		float mag = vector.Magnitude();
		ASSERT(mag != 0, "Impossible to normalize! Magnitude equal to zero");
		return Vec3(vector.x / mag, vector.y / mag, vector.z / mag);
	}

	Vec3 Lerp(const Vec3& v0, const Vec3& v1, const float time)
	{
		return v0 * (1 - time) + v1 * time;
	}

	Vec3 Reflect(const Vec3& vectorDir, const Vec3& normal)
	{
		return vectorDir - normal * DotProduct(normal, vectorDir) * 2;
		//return (2 * DotProduct(-vectorDir, normal)) * normal + vectorDir;
	}

	Vec3 Refract(const Vec3& incident, const Vec3& normal, const float refractionIndex)
	{
		float dotValue = DotProduct(normal, incident);
		float k = 1.f - refractionIndex * refractionIndex * (1 - dotValue * dotValue);
		Vec3 result = (k > 0.f) ? (refractionIndex * incident - (refractionIndex * dotValue + std::sqrtf(k)) * normal) : -1 * incident;
		return result;
	}

	Vec3& Vec3::Normalized()
	{
		float mag = this->Magnitude();
		ASSERT(mag != 0, "Impossible to normalize! Magnitude equal to zero");
		for (uint32_t i = 0; i < 3; i++)
			components[i] = components[i] / mag;

		return *this;
	}

	float Vec3::DotProduct(const Vec3& other) const
	{
		return (this->x * other.x + this->y * other.y + this->z * other.z);
	}

	Vec3 Vec3::CrossProduct(const Vec3& other) const
	{
		float x = (this->y * other.z) - (this->z * other.y);
		float y = (this->z * other.x) - (this->x * other.z);
		float z = (this->x * other.y) - (this->y * other.x);
		return Vec3(x, y, z);
	}

	Vec3& Vec3::ClampColor()
	{
		
		r = std::clamp(r, 0.f, 1.f);
		g = std::clamp(g, 0.f, 1.f);
		b = std::clamp(b, 0.f, 1.f);
		return  *this;
		
	}

	Vec3 Vec3::operator-() const
	{
		return -1.f * (*this);
	}

	Vec3& Vec3::operator+=(const Vec3& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}

	Vec3& Vec3::operator-=(const Vec3& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		return *this;
	}

	Vec3& Vec3::operator*=(const float& scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		this->z *= scalar;
		return *this;
	}

	Vec3& Vec3::operator=(const Vec3& other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}

	Vec3& Vec3::operator=(Vec3&& other) noexcept
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}

	float Vec3::operator[](const uint32_t index) const
	{
		return components[index];
	}

	float& Vec3::operator[](uint32_t index)
	{
		return components[index];
	}

	float DotProduct(const Vec3& v0, const Vec3& v1)
	{
		return (v0.x * v1.x + v0.y * v1.y + v0.z * v1.z);
	}

	Vec3 CrossProduct(const Vec3& v0, const Vec3& v1)
	{
		float x = (v0.y * v1.z) - (v0.z * v1.y);
		float y = (v0.z * v1.x) - (v0.x * v1.z);
		float z = (v0.x * v1.y) - (v0.y * v1.x);
		return Vec3(x, y, z);
	}

	Vec3 ProjectOnPlane(const Vec3& vectorDir, const Vec3& normal)
	{
		return vectorDir - normal * DotProduct(vectorDir, normal);
	}

	float Distance(const Vec3& v0, const Vec3& v1)
	{
		return Magnitude(v0 - v1);
	}

	const Vec3 operator+(const Vec3& left, const Vec3& right)
	{
		return Vec3(left.x + right.x, left.y + right.y, left.z + right.z);
	}

	const Vec3 operator-(const Vec3& left, const Vec3& right)
	{
		return Vec3(left.x - right.x, left.y - right.y, left.z - right.z);
	}

	const Vec3 operator*(const Vec3& vector, const float scalar)
	{
		return Vec3(vector.x * scalar, vector.y * scalar, vector.z * scalar);
	}

	const Vec3 operator*(const float scalar, const Vec3& vector)
	{
		return Vec3(vector.x * scalar, vector.y * scalar, vector.z * scalar);
	}

	const Vec3 operator*(const Vec3& v1, const Vec3& v2)
	{
		return Vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
	}

	const Vec3 operator/(const Vec3& left, const float right)
	{
		return Vec3(left.x / right, left.y / right, left.z / right);
	}

	const Vec3 operator/(const Vec3& left, const Vec3& right)
	{
		return Vec3(left.x / right.x, left.y / right.y, left.z / right.z);
	}

	const Vec3 operator/(const Vec3& left, const Vec3Int& right)
	{
		return Vec3(left.x / right.x, left.y / right.y, left.z / right.z);
	}

	const bool operator==(const Vec3& left, const Vec3& right)
	{
		return fabs(left.x - right.x) <= EPSILON && fabs(left.y - right.y) <= EPSILON && fabs(left.z - right.z) <= EPSILON;
	}

	const bool operator!=(const Vec3& left, const Vec3& right)
	{
		return !(left == right);
	}

	std::ostream& operator<<(std::ostream& stream, const Vec3& Vector)
	{
		stream << "Vector 3D: (" << Vector.x << ", " << Vector.y << ", " << Vector.z << ")";
		return stream;
	}

	std::istream& operator>>(std::istream& stream, Vec3& Vector)
	{
		return stream >> Vector.x >> Vector.y >> Vector.z;
	}

	/**/
	// Sampling with rejection method
	Vec3 sample_unit_disk()
	{
		Vec3 p;
		do
		{
			p = Vec3(Random::Float(), Random::Float(), 0.0) * 2 - Vec3(1.0, 1.0, 0.0);
		} while (p.DotProduct(p) >= 1.0);
		return p;
	}

#pragma endregion Vec3
	/////////////////////////////////////////////////////////////////////// Vec3Int
#pragma region Vec3Int
	Vec3Int::Vec3Int()
		: x(0), y(0), z(0)
	{
	}
	Vec3Int::Vec3Int(const int& k)
		: x(k), y(k), z(k)
	{
	}
	Vec3Int::Vec3Int(const int& x, const int& y, const int& z)
		: x(x), y(y), z(z)
	{
	}
	Vec3Int::Vec3Int(const Vec3Int& other)
		: x(other.x), y(other.y), z(other.z)
	{
	}
	Vec3Int::Vec3Int(Vec3Int&& other) noexcept
		: x(other.x), y(other.y), z(other.z)
	{
	}
	int* Vec3Int::Data()
	{
		return &x;;
	}
	const int* Vec3Int::Data() const
	{
		return &x;
	}

	const Vec3Int operator+(const Vec3Int& left, const Vec3Int& right)
	{
		return Vec3Int(left.x + right.x, left.y + right.y, left.z + right.z);
	}

	const Vec3Int operator-(const Vec3Int& left, const Vec3Int& right)
	{
		return Vec3Int(left.x - right.x, left.y - right.y, left.z - right.z);
	}

	const Vec3Int operator*(const Vec3Int& vector, const int scalar)
	{
		return Vec3Int(vector.x * scalar, vector.y * scalar, vector.z * scalar);
	}

	const Vec3Int operator*(const int scalar, const Vec3Int& vector)
	{
		return Vec3Int(vector.x * scalar, vector.y * scalar, vector.z * scalar);
	}

	const Vec3Int operator*(const Vec3Int& v1, const Vec3Int& v2)
	{
		return Vec3Int(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
	}

	const Vec3Int operator/(const Vec3Int& left, const int right)
	{
		return Vec3Int(left.x / right, left.y / right, left.z / right);
	}

	const Vec3Int operator/(const Vec3Int& left, const Vec3Int& right)
	{
		return Vec3Int(left.x / right.x, left.y / right.y, left.z / right.z);
	}

	const bool operator==(const Vec3Int& left, const Vec3Int& right)
	{
		return abs(left.x - right.x) <= EPSILON && abs(left.y - right.y) <= EPSILON && abs(left.z - right.z) <= EPSILON;
	}

	const bool operator!=(const Vec3Int& left, const Vec3Int& right)
	{
		return !(left == right);
	}

	std::ostream& operator<<(std::ostream& stream, const Vec3Int& Vector)
	{
		stream << "Vector 3D: (" << Vector.x << ", " << Vector.y << ", " << Vector.z << ")";
		return stream;
	}

	std::istream& operator>>(std::istream& stream, Vec3Int& Vector)
	{
		return stream >> Vector.x >> Vector.y >> Vector.z;
	}

	Vec3Int Vec3Int::operator-() const
	{
		return -1 * (*this);
	}

	Vec3Int& Vec3Int::operator+=(const Vec3Int& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}

	Vec3Int& Vec3Int::operator-=(const Vec3Int& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		return *this;
	}

	Vec3Int& Vec3Int::operator*=(const int& scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		this->z *= scalar;
		return *this;
	}

	Vec3Int& Vec3Int::operator=(const Vec3Int& other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}

	Vec3Int& Vec3Int::operator=(Vec3Int&& other) noexcept
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}

	int Vec3Int::operator[](const uint32_t index) const
	{
		return components[index];
	}

	int& Vec3Int::operator[](uint32_t index)
	{
		return components[index];
	}
	/**/
#pragma endregion Vec3Int
}
