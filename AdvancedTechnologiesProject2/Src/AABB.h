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
	bool checkRayCollision(const Ray* ray, float& t) const;

	void setMin(const glm::vec3& min) { m_bounds[0] = min; }
	void setMax(const glm::vec3& max) { m_bounds[1] = max; }

	const glm::vec3& getMin() const { return m_bounds[0]; }
	const glm::vec3& getMax() const { return m_bounds[1]; }
	const float& getSurfaceArea() const;
private: 
	glm::vec3 m_bounds[2];
};
