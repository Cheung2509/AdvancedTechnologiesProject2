#include "Shape.h"

Shape::Shape(const glm::vec3 & surfCol, const glm::vec3 & emisCol, const float & refl, const float & transp)
	: m_surfaceColour(surfCol), m_emissionColour(emisCol), m_reflection(refl), m_transparency(transp)
{
}
