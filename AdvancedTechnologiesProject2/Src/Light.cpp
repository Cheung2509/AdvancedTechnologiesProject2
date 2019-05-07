#include "Light.h"

#include "glm/gtc/constants.hpp"

void Light::illuminate(const glm::vec3& p, glm::vec3& dir, glm::vec3& intensity, float& distance)
{
	dir = glm::normalize(p - m_pos);
	float r2 = dir.x *dir.x + dir.y * dir.y + dir.z * dir.z;
	distance = dir.length();

	dir.x /= distance;
	dir.y /= distance;
	dir.z /= distance;

	intensity = m_colour * m_intensity / (4 * glm::pi<float>() * r2);
}
