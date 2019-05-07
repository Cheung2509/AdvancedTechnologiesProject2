#pragma once

#include "Object.h"

class Light : public Object
{
public:
	Light() {};
	~Light() {};

	void illuminate(const glm::vec3& p, glm::vec3& dir, glm::vec3& intensity, float& distance);

	const float& getIntensity() const { return m_intensity; }
	void setIntensity(const float& intensity) { m_intensity = intensity; }
private:
	float m_intensity = 1.0f;
	glm::vec3 m_colour = glm::vec3(1.0f);
};