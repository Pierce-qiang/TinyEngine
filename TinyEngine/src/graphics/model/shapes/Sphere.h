#pragma once
#include "../Mesh.h"
namespace TEngine {
	class Sphere : public Mesh
	{
	public:
		Sphere(int xSegments = 30, int ySegments = 30);
		~Sphere();
	};
}


