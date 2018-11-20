#pragma once
#include <glm/glm.hpp>

class Ray;

class AABB
{
public:
	AABB() = default;
	AABB(glm::vec3 min, glm::vec3 max);
	~AABB() = default;

	bool checkRayCollision(const Ray* ray) const;
private: 
	glm::vec3 m_bounds[2];
};
