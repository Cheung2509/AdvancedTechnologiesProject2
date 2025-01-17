#include "Sphere.h"

#include <corecrt_math_defines.h>

#include <iostream>

#include "Ray.h"

Sphere::Sphere(const float & r, const glm::vec3 & colour, const glm::vec3& pos)
	:m_radius(r), m_radius2(r*r), Geometry(colour, GeometryType::PRIMITIVE)
{
	m_pos = pos;
	m_boundingBox = AABB(glm::vec3(-m_radius) + m_pos, glm::vec3(m_radius) + m_pos);
}

bool Sphere::intersect(Ray* ray, std::uint64_t & index, glm::vec2 & uv, float& t)
{
	//2 positions that could be intersected
	float t0;
	float t1;

	//geometric solution to intersection check for spheres
	glm::vec3 l = m_pos - ray->getOrigin();
	float tca = glm::dot(l, ray->getDirection());
	if (tca < 0)
		return false;

	float d2 = glm::dot(l, l) - tca * tca;
	if (d2 > m_radius2)
		return false;

	float thc = glm::sqrt(m_radius2 - d2);
	t0 = tca - thc;
	t1 = tca + thc;

	if(t0 < 0 && t1 < 0)
	{
		return false;
	}

	//Get the closest intersection
	if (t0 > t1)
	{
		std::swap(t0, t1);
	}


	if (t0 < ray->getClosestHit())
	{
		//Set the object the ray hit as this one as it is closer
		ray->setHit(t0);
	}

	if (t0 < t)
	{
		t = t0;
	}

	return true;
}

void Sphere::getSurfaceData(const glm::vec3& pHit, glm::vec3& nHit, glm::vec2& tex) const
{
	nHit = glm::normalize(pHit - m_pos);

	tex.x = (1 + atan2(nHit.z, nHit.x) / M_PI) * 0.5f;
	tex.y = acosf(nHit.y) / M_PI;
}

void Sphere::getSurface(const glm::vec3 & p, const glm::vec3 & i, const uint64_t & index, const glm::vec2 & uv, glm::vec3 & n, glm::vec2 & st) const
{
	n = glm::normalize(p - m_pos);

	st.x = (1 + atan2(n.z, n.x) / M_PI) * 0.5f;
	st.y = acosf(n.y) / M_PI;
}
