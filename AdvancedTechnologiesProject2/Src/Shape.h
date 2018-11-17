#pragma once

#include "Actor.h"

class Shape : public Actor
{
public:
	Shape() = default;
	Shape(const glm::vec3& surfCol, const glm::vec3& emisCol,
		  const float& refl, const float& transp);
	virtual ~Shape() = default;

	virtual bool intersect(const glm::vec3 & rayorig, const glm::vec3 & rayDir, float & t0, 
				   float & t1) const = 0;

	const float& getTransparency() const { return m_transparency; }
	const float& getReflecation() const { return m_reflection; }

	const glm::vec3& getSurfaceColour() const { return m_surfaceColour; }
	const glm::vec3& getEmissionColour() const { return m_emissionColour; }

protected:
	glm::vec3 m_surfaceColour;
	glm::vec3 m_emissionColour;

	float m_reflection = 0;
	float m_transparency = 0;
};