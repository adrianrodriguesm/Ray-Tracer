#pragma once
#include "Math/Vector.h"
#include "Renderer/Color.h"
namespace rayTracer
{
	class Light
	{
	public:

		Light(Vector& pos, Color& col) : position(pos), color(col) {};
		Vector position;
		Color color;
	};

}