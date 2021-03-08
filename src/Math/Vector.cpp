#include "Vector.h"
#include "Math/Maths.h"

namespace rayTracer
{
	Vector::Vector(float a_x, float a_y, float a_z) : x(a_x), y(a_y), z(a_z)
	{

	}


	float Vector::length()
	{
		return sqrt(x * x + y * y + z * z);
	}

	// --------------------------------------------------------------------- copy constructor
	Vector::Vector(const Vector& v)
	{
		x = v.x; y = v.y; z = v.z;
	}

	// --------------------------------------------------------------------- assignment operator
	Vector Vector::operator= (const Vector& rhs) {
		if (this == &rhs)
			return (*this);
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return (*this);
	}

	Vector Vector::operator+(const  Vector& v)
	{
		return Vector(x + v.x, y + v.y, z + v.z);
	}


	Vector Vector::operator-(const Vector& v)
	{
		return Vector(x - v.x, y - v.y, z - v.z);
	}

	float Vector::operator*(const  Vector& v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	Vector Vector::operator/(float f)
	{
		return Vector(x / f, y / f, z / f);
	}

	Vector& Vector::normalize()
	{
		float l = 1.0 / this->length();
		x *= l; y *= l; z *= l;
		return *this;
	}

	Vector& Vector::operator-=(const Vector& v)
	{
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}

	Vector& Vector::operator-=(const float v)
	{
		x -= v; y -= v; z -= v; return *this;
	}

	Vector& Vector::operator+=(const float v)
	{
		x += v; y += v; z += v; return *this;
	}

	Vector& Vector::operator*=(const float v)
	{
		x *= v; y *= v; z *= v; return *this;
	}

	Vector Vector::operator%(const Vector& v)
	{
		float uX = x;
		float uY = y;
		float uZ = z;

		float vX = v.x;
		float vY = v.y;
		float vZ = v.z;

		float sX = uY * vZ - uZ * vY;
		float sY = uZ * vX - uX * vZ;
		float sZ = uX * vY - uY * vX;

		return Vector(sX, sY, sZ);
	}

	// Sampling with rejection method
	Vector sample_unit_disk(void) {
		Vector p;
		do {
			p = Vector(rand_float(), rand_float(), 0.0) * 2 - Vector(1.0, 1.0, 0.0);
		} while (p * p >= 1.0);
		return p;
	}
	Vector operator*(const Vector& v, const float f)
	{
		return Vector(v.x * f, v.y * f, v.z * f);
	}
	Vector operator*(const float f, const Vector& v)
	{
		return Vector(v.x * f, v.y * f, v.z * f);
	}
}
