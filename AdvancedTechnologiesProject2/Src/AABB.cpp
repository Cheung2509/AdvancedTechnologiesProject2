#include "AABB.h"

#include "Ray.h"

AABB::AABB(glm::vec3 min, glm::vec3 max)
{
	m_bounds[0] = min;
	m_bounds[1] = max;
}

bool AABB::checkRayCollision(const Ray * ray) const
{
	float tmin = (m_bounds[ray->getSign(0)].x - ray->getOrigin().x) * ray->getInvDirection().x;
	float tmax = (m_bounds[1 - ray->getSign(0)].x - ray->getOrigin().x) * ray->getInvDirection().x;
	float tymin = (m_bounds[ray->getSign(1)].y - ray->getOrigin().y) * ray->getInvDirection().y;
	float tymax = (m_bounds[1 - ray->getSign(1)].y - ray->getOrigin().y) * ray->getInvDirection().y;

	if (tmin > tymax || tymin > tmax)
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (m_bounds[ray->getSign(2)].z - ray->getOrigin().z)  * ray->getInvDirection().z;
	float tzmax = (m_bounds[1 - ray->getSign(2)].z - ray->getOrigin().z) * ray->getInvDirection().z;

	if (tmin > tzmax || tzmin > tmax)
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}

bool AABB::checkRayCollision(const Ray * ray, float & t) const
{
	float tmin = (m_bounds[ray->getSign(0)].x - ray->getOrigin().x) * ray->getInvDirection().x;
	float tmax = (m_bounds[1 - ray->getSign(0)].x - ray->getOrigin().x) * ray->getInvDirection().x;
	float tymin = (m_bounds[ray->getSign(1)].y - ray->getOrigin().y) * ray->getInvDirection().y;
	float tymax = (m_bounds[1 - ray->getSign(1)].y - ray->getOrigin().y) * ray->getInvDirection().y;

	if (tmin > tymax || tymin > tmax)
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (m_bounds[ray->getSign(2)].z - ray->getOrigin().z)  * ray->getInvDirection().z;
	float tzmax = (m_bounds[1 - ray->getSign(2)].z - ray->getOrigin().z) * ray->getInvDirection().z;

	if (tmin > tzmax || tzmin > tmax)
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	t = tmin;
	 
	if (t < 0)
	{
		t = tmax;
		if (t < 0)
			return false;
	}

	return true;
}

const float & AABB::getSurfaceArea() const
{
	float l = glm::abs(m_bounds[0].z - m_bounds[1].z);
	float w = glm::abs(m_bounds[0].x - m_bounds[1].x);
	float h = glm::abs(m_bounds[0].y - m_bounds[1].y);

	return 2 * ((l * w) + (l * h) + (w * h));
}
