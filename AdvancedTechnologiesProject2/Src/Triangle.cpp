#include "Triangle.h"

#include <corecrt_math_defines.h>

#include "Ray.h"

constexpr float kEpsilon = 1e-8;

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 col)
	: m_v0(v0), m_v1(v1), m_v2(v2), Geometry(col, GeometryType::PRIMITIVE)
{
	glm::vec3 min = glm::min(m_v0, m_v1);
	min = glm::min(min, m_v2);

	glm::vec3 max = glm::max(m_v0, m_v1);
	max = glm::max(max, m_v2);

	glm::vec3 centre = glm::abs(min - max);

	m_pos = (centre / 2.0f) + min;

	m_boundingBox = AABB(min, max);
}

bool Triangle::intersect(Ray * ray, std::uint64_t & index, glm::vec2 & uv, float & t)
{
	//Using Moller Trumbore algorithm
	glm::vec3 v0v1 = m_v1 - m_v0;
	glm::vec3 v0v2 = m_v2 - m_v0;
	glm::vec3 pVec = glm::cross(ray->getDirection(), v0v2);
	float det = glm::dot(v0v1, pVec);

	float invDet = 1 / det;

	if (det < kEpsilon)
		return false;

	glm::vec3 tvec = ray->getOrigin() - m_v0;
	uv.x = glm::dot(tvec, pVec) * invDet;
	if (uv.x < 0 || uv.x > 1)
		return false;

	glm::vec3 qVec = glm::cross(tvec, v0v1);
	uv.y = glm::dot(ray->getDirection(), qVec) * invDet;
	if (uv.y < 0 || uv.x + uv.y > 1)
		return false;

	t = glm::dot(v0v2, qVec) * invDet;

	if (ray->getClosestHit() > t)
	{
		ray->setHit(t);
	}

	return true;
}

void Triangle::getSurfaceData(const glm::vec3 & pHit, glm::vec3 & nHit, glm::vec2 & tex) const
{
}

void Triangle::getSurfaceProperties(const glm::vec3& hitPoint, const glm::vec3& viewDirection,
									const uint64_t& index, const glm::vec2& uv, glm::vec3& hitNormal,
									glm::vec2& hitTextureCoordinate) const
{
	hitNormal = glm::cross((m_v1 - m_v0), (m_v2 - m_v0));
	hitNormal = glm::normalize(hitNormal);
}
