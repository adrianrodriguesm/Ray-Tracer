#pragma once

#include "Math/Matrix.h"

#define OUT

namespace rayTracer
{
	struct Qtrn {

		float t, x, y, z;
		
		Qtrn();
		Qtrn(float t, float x, float y, float z);
		Qtrn(float theta,const Vec3& axis);

		float* data();
		
		friend const void toAngleAxis(const Qtrn& q, OUT float& theta, OUT Vec3& axis);
		friend const void clean(Qtrn& q);
		friend const float quadrance(const Qtrn& q);
		friend const float norm(const Qtrn& q);
		friend const Qtrn normalize(const Qtrn& q);
		friend const Qtrn conjugate(const Qtrn& q);
		friend const Qtrn inverse(const Qtrn& q);

		Qtrn& operator = (const Qtrn& q);
		friend const Qtrn operator + (const Qtrn& q0, const Qtrn& q1);
		friend const Qtrn operator * (const Qtrn& q, const float s);
		friend const Qtrn operator * (const float s, const Qtrn& q);
		friend const Qtrn operator * (const Qtrn& q0, const Qtrn& q1);
		friend const bool operator == (const Qtrn& q0, const Qtrn& q1);
		friend const bool operator != (const Qtrn& q0, const Qtrn& q1);

		friend const Mat4 GLRotationMatrix(const Qtrn& q);
		friend const Qtrn lerp(const Qtrn& q0, const Qtrn& q1,const float k);
		friend const Qtrn slerp(const Qtrn& q0, const Qtrn& q1, const float k);
		
		friend std::ostream& operator << (std::ostream& os, const Qtrn& q);
		friend const void printAngleAxis(const std::string& s, const Qtrn& q);
	};
}