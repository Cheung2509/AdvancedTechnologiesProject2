#include "Sphere.h"

Sphere::Sphere(const float & r, const glm::vec3 & sCol, const glm::vec3 eCol,
			   const float & refl, const float& transp)
	:m_radius(r), m_radius2(r*r), Shape(sCol, eCol, refl, transp)
{
}

bool Sphere::intersect(const glm::vec3 & rayorig, const glm::vec3 & rayDir, float & t0, float & t1) const
{
	glm::vec3 l = m_pos - rayorig;
	float tca = glm::dot(l, rayDir);

	if (tca < 0)
	{
		return false;
	}

	float d2 = glm::dot(l, l) - tca * tca;
	if (d2 > m_radius2)
	{
		return false;
	}

	float thc = glm::sqrt(m_radius2 - d2);
	t0 = tca - thc;
	t1 = tca + thc;

	return true;
}
