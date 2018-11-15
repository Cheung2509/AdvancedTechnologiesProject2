#pragma once

#include "Actor.h"

class Shape : public Actor
{
public:
	Shape() = default;
	virtual ~Shape() = default;

	virtual bool intersect(const glm::vec3 & rayorig, const glm::vec3 & rayDir, float & t0, 
				   float & t1) const = 0;

protected:
	glm::vec4 m_surfaceColour;
	glm::vec4 m_emissionColour;

	float m_reflection = 0;
};