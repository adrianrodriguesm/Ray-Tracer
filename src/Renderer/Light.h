#pragma once
#include "Math/Vector.h"
#include "Renderer/Color.h"
namespace rayTracer
{
	class Light
	{
	public:

		Light(Vec3& pos, Vec3& col) : position(pos), color(col) {};
		~Light() = default;
		Vec3 position;
		Vec3 color;
	};

}