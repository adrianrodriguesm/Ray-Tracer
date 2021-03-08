#include "BoundingBox.h"

namespace rayTracer
{
	/** /
	//-------------------------------------------------------------------- - default constructor
	AABB::AABB()
	{
		Min = Vector(-1.0f, -1.0f, -1.0f);
		Max = Vector(1.0f, 1.0f, 1.0f);
	}
	// --------------------------------------------------------------------- assignment operator
	AABB AABB::operator= (const AABB& rhs) 
	{
		if (this == &rhs)
			return (*this);
		Min = rhs.Min;
		Max = rhs.Max;
		return (*this);
	}

	// --------------------------------------------------------------------- inside
	// used to test if a ray starts inside a grid
	bool AABB::IsInside(const Vector& p)
	{
		return ((p.x > Min.x && p.x < Max.x) && (p.y > Min.y && p.y < Max.y) && (p.z > Min.z && p.z < Max.z));
	}
	/**/
}