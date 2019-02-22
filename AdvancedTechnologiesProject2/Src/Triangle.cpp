#include "Triangle.h"

#include <corecrt_math_defines.h>

#include "Ray.h"

constexpr float kEpsilon = 1e-8;

Triangle::Triangle(std::shared_ptr<glm::vec3> v0, std::shared_ptr<glm::vec3> v1, std::shared_ptr<glm::vec3> v2, 
				   std::shared_ptr<glm::vec3> n0, std::shared_ptr<glm::vec3> n1, std::shared_ptr<glm::vec3> n2, glm::vec3 col)
	: m_v0(v0), m_v1(v1), m_v2(v2), m_n0(n0), m_n1(n1), m_n2(n2), Geometry(col, GeometryType::PRIMITIVE)
{
	glm::vec3 min(kInfinity);
	min = glm::min(min, *m_v0);
	min= glm::min(min, *m_v1);
	min = glm::min(min, *m_v2);

	glm::vec3 max(-kInfinity);
	max = glm::max(max, *m_v0);
	max = glm::max(max, *m_v1);
	max = glm::max(max, *m_v2);

	m_pos = glm::vec3((min + max) * 0.5f);

	m_boundingBox = AABB(min, max);
}

Triangle::Triangle(std::shared_ptr<glm::vec3> v0, std::shared_ptr<glm::vec3> v1, std::shared_ptr<glm::vec3> v2, glm::vec3 col)
	: m_v0(v0), m_v1(v1), m_v2(v2), Geometry(col, GeometryType::PRIMITIVE)
{
	glm::vec3 min(kInfinity);
	min = glm::min(min, *m_v0);
	min = glm::min(min, *m_v1);
	min = glm::min(min, *m_v2);

	glm::vec3 max(-kInfinity);
	max = glm::max(max, *m_v0);
	max = glm::max(max, *m_v1);
	max = glm::max(max, *m_v2);

	m_pos = glm::vec3((min + max) * 0.5f);

	m_boundingBox = AABB(min, max);
}

bool Triangle::intersect(Ray * ray, std::uint64_t & index, glm::vec2 & uv, float & t)
{
	//Using Moller Trumbore algorithm
	glm::vec3 v0v1 = *m_v1 - *m_v0;
	glm::vec3 v0v2 = *m_v2 - *m_v0;
	glm::vec3 pVec = glm::cross(ray->getDirection(), v0v2);
	float det = glm::dot(v0v1, pVec);

	if (det < kEpsilon)
		return false;

	float invDet = 1 / det;

	glm::vec3 tvec = ray->getOrigin() - *m_v0;
	float u = glm::dot(tvec, pVec) * invDet;
	if (u < 0 || u > 1)
		return false;

	glm::vec3 qVec = glm::cross(tvec, v0v1);
	float v = glm::dot(ray->getDirection(), qVec) * invDet;
	if (v < 0 || u + v > 1)
		return false;

	t = glm::dot(v0v2, qVec) * invDet;

	if (ray->getClosestHit() > t)
	{
		ray->setHit(t);
		uv = glm::vec2(u, v);
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
	hitNormal = glm::cross((*m_v1 - *m_v0), (*m_v2 - *m_v0));
	hitNormal = glm::normalize(hitNormal);

	/*hitNormal = (1 - uv.x - uv.y) * m_n0 +
		uv.x * m_n1 + uv.y * m_n2;*/
}
