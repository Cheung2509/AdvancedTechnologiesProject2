#include "Sphere.h"

#include <corecrt_math_defines.h>

#include <iostream>

Sphere::Sphere(const float & r, const glm::vec3 & colour)
	:m_radius(r), m_radius2(r*r), Shape(colour)
{
}

bool Sphere::intersect(const glm::vec3 & orig, const glm::vec3 & dir, float & tnear, std::uint64_t & index, glm::vec2 & uv)
{
	float t0;
	float t1;

	glm::vec3 l = m_pos - orig;
	float tca = glm::dot(l, dir);
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

	if (t0 > t1)
	{
		std::swap(t0, t1);
	}

	if(t0 < 0)
	{
		t0 = t1;
		if (t0 < 0)
		{
			return false;
		}
	}

	tnear = t0;

	return true;
}

void Sphere::getSurfaceData(const glm::vec3& pHit, glm::vec3& nHit, glm::vec2& tex) const
{
	nHit = glm::normalize(pHit - m_pos);

	tex.x = (1 + atan2(nHit.z, nHit.x) / M_PI) * 0.5f;
	tex.y = acosf(nHit.y) / M_PI;
}

void Sphere::getSurfaceProperties(const glm::vec3 & p, const glm::vec3 & i, const uint64_t & index, const glm::vec2 & uv, glm::vec3 & n, glm::vec2 & st) const
{
	n = glm::normalize(p - m_pos);
}
