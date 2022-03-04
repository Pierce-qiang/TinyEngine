#include "Sphere.h"
#include "glm/gtc/constants.hpp"
namespace TEngine {
	Sphere::Sphere(int xSegments, int ySegments)
	{
		for (int y = 0; y <= ySegments; ++y)
		{
			for (int x = 0; x <= xSegments; ++x)
			{
				float xSegment = (float)x / (float)xSegments;
				float ySegment = (float)y / (float)ySegments;
				float xPos = std::cos(xSegment * glm::pi<float>() * 2.0f) * std::sin(ySegment * glm::pi<float>());
				float yPos = std::cos(ySegment * glm::pi<float>());
				float zPos = std::sin(xSegment * glm::pi<float>() * 2.0f) * std::sin(ySegment * glm::pi<float>());

				mPositions.push_back(glm::vec3(xPos, yPos, zPos));
				mUVs.push_back(glm::vec2(xSegment, ySegment));
				mNormals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		for (int y = 0; y < ySegments; ++y)
		{
			for (int x = 0; x < xSegments; ++x)
			{
				mIndices.push_back((y + 1) * (xSegments + 1) + x);
				mIndices.push_back(y * (xSegments + 1) + x);
				mIndices.push_back(y * (xSegments + 1) + x + 1);

				mIndices.push_back((y + 1) * (xSegments + 1) + x);
				mIndices.push_back(y * (xSegments + 1) + x + 1);
				mIndices.push_back((y + 1) * (xSegments + 1) + x + 1);
			}
		}

		SetupMesh();
	}

	Sphere::~Sphere() {}

}