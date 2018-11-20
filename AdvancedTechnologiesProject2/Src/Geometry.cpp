#include "Geometry.h"

#include <algorithm>

#include "Ray.h"


Geometry::Geometry(const glm::vec3 & col, MaterialType type, float ior, float kd, float ks, float specular)
	:m_diffuseColour(col), m_matType(type), m_ior(ior), m_kd(kd), m_ks(ks), m_specularExponent(specular)
{
}

bool Geometry::solveQuadratic(const float & a, const float & b, float & c, float & x0, float & x1)
{
	float discr = b * b - 4 * a * c;

	if (discr < 0)
		return false;
	else if (discr == 0)
		x0 = x1 = -0.5f * b / a;
	else
	{
		float q = (b > 0) ? 
			-0.5f *(b + glm::sqrt(discr)) : 
			-0.5f *(b - glm::sqrt(discr));

		x0 = q / a;
		x1 = c / q;
	}
	
	if (x0 > x1)
		std::swap(x0, x1);

	return true;
}
