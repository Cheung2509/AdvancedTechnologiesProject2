#pragma once

#include <glm/glm.hpp>
#include <vector>

class Shape;

class Ray
{
public:
	Ray();
	~Ray();

	const glm::vec4& trace(const glm::vec3& rayorig, const glm::vec3& rayDir,
						   const std::vector<Shape>& shapes, const int&depth);
private:
};