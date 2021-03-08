#pragma once

#include <iostream>
namespace rayTracer
{
	struct Vector
	{
		float x, y, z;
		Vector() {};
		Vector(float x, float y, float z);
		Vector(const Vector& v);

		float length();
		Vector& normalize();
		Vector operator=(const Vector& v);
		Vector operator+(const Vector& v);
		Vector operator-(const Vector& v);

		friend Vector operator*(const Vector& v, const float f);
		friend Vector operator*( const float f, const Vector& v);

		float  operator*(const Vector& v);   //inner product
		Vector operator/(float f);
		Vector operator%(const Vector& v); //external product
		Vector& operator-=	(const Vector& v);
		Vector& operator-=	(const float v);
		Vector& operator*=	(const float v);
		Vector& operator+=	(const float v);

		
		


		Vector sample_unit_disk(void);

		friend inline std::istream& operator >>	(std::istream& s, Vector& v)
		{
			return s >> v.x >> v.y >> v.z;
		}

		friend std::ostream& operator<<(std::ostream& stream, const Vector& Vector)
		{
			stream << "Vector 3D: (" << Vector.x << ", " << Vector.y << ", " << Vector.z << ")";
			return stream;
		}

	};


	Vector operator*(const Vector& vector, const float scalar);
	Vector operator*(const float scalar, const Vector& vector);
	
	
}
