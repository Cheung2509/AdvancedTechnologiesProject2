#pragma once

#include <SFML/System.hpp>

#include <glm/glm.hpp>

#include "Shape.h"

class Sphere : public Shape
{
public:
	Sphere() = default;
	Sphere(const float& r, const glm::vec4& sCol, 
		   const glm::vec4 eCol = glm::vec4(0.0f,0.0f,0.0f,1.0f),
		   const float& refl = 0);
	~Sphere() = default;

	bool intersect(const glm::vec3& rayorig, const glm::vec3& rayDir, float &t0, float& t1) const;


private:
	float m_radius;
	float m_radius2;
	glm::vec4 m_surfaceColour;
	glm::vec4 m_emissionColour;

	float m_reflection = 0;
};