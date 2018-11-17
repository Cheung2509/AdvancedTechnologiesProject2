#pragma once

#include <glm/glm.hpp>
#include <vector>

class Shape;

class Ray
{
public:
	Ray() = default;
	~Ray() = default;

	static const glm::vec3& trace(const glm::vec3& rayorig, const glm::vec3& rayDir,
						   const std::vector<Shape*>& shapes, const int&depth);
	static const float& mix(const float& a, const float& b, const float& mix);
private:
};