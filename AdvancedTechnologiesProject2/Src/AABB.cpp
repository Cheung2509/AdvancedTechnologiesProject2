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
	float tmax = (1 - m_bounds[ray->getSign(0)].x - ray->getOrigin().x) * ray->getInvDirection().x;
	float tymin = (m_bounds[ray->getSign(1)].y - ray->getOrigin().y) * ray->getInvDirection().y;
	float tymax = (1 - m_bounds[ray->getSign(1)].y - ray->getOrigin().y) * ray->getInvDirection().y;

	if (tmin > tymax || tymin > tmax)
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (m_bounds[ray->getSign(2)].z - ray->getOrigin().z)  * ray->getInvDirection().z;
	float tzmax = (1 - m_bounds[ray->getSign(2)].z - ray->getOrigin().z) * ray->getInvDirection().z;

	if (tmin > tzmax || tzmin > tmax)
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}
