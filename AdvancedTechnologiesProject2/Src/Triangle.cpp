#include "Triangle.h"

#include <corecrt_math_defines.h>

#include "Ray.h"

constexpr float kEpsilon = 1e-8;

Triangle::Triangle(std::shared_ptr<glm::vec3> v0, std::shared_ptr<glm::vec3> v1, std::shared_ptr<glm::vec3> v2, 
				   std::shared_ptr<glm::vec3> n0, std::shared_ptr<glm::vec3> n1, std::shared_ptr<glm::vec3> n2, 
				   std::shared_ptr<glm::vec2> uv0, std::shared_ptr<glm::vec2> uv1, std::shared_ptr<glm::vec2> uv2, 
				   glm::vec3 col)
	: Geometry(col, GeometryType::PRIMITIVE)
{
	m_v0 = {*v0 , *n0, *uv0};
	m_v1 = {*v1 , *n1, *uv1};
	m_v2 = {*v2 , *n2, *uv2};

	glm::vec3 min(kInfinity);
	min = glm::min(min, m_v0.m_pos);
	min= glm::min(min, m_v1.m_pos);
	min = glm::min(min, m_v2.m_pos);

	glm::vec3 max(-kInfinity);
	max = glm::max(max, m_v0.m_pos);
	max = glm::max(max, m_v1.m_pos);
	max = glm::max(max, m_v2.m_pos);

	m_pos = glm::vec3((min + max) * 0.5f);

	//glm::mat4 transFormNormal = glm::transpose(glm::inverse(this->getWorld()));
	//*m_n0 = glm::normalize(transFormNormal * glm::vec4(*m_n0, 1.0f));
	//*m_n1 = glm::normalize(transFormNormal * glm::vec4(*m_n1,1.0f));
	//*m_n2 = glm::normalize(transFormNormal * glm::vec4(*m_n2, 1.0f));


	m_boundingBox = AABB(min, max);
}

Triangle::Triangle(std::shared_ptr<glm::vec3> v0, std::shared_ptr<glm::vec3> v1, std::shared_ptr<glm::vec3> v2, glm::vec3 col)
	:Geometry(col, GeometryType::PRIMITIVE)
{
	glm::vec3 min(kInfinity);
	min = glm::min(min, m_v0.m_pos);
	min = glm::min(min, m_v1.m_pos);
	min = glm::min(min, m_v2.m_pos);

	glm::vec3 max(-kInfinity);
	max = glm::max(max, m_v0.m_pos);
	max = glm::max(max, m_v1.m_pos);
	max = glm::max(max, m_v2.m_pos);

	m_v0.m_normal = glm::normalize(m_v0.m_pos);
	m_v1.m_normal = glm::normalize(m_v1.m_pos);
	m_v2.m_normal = glm::normalize(m_v2.m_pos);

	m_pos = glm::vec3((min + max) * 0.5f);

	m_boundingBox = AABB(min, max);
}

bool Triangle::intersect(Ray * ray, std::uint64_t & index, glm::vec2 & uv, float & t)
{
	//Using Moller Trumbore algorithm

	glm::vec3 v0v1 = m_v1.m_pos - m_v0.m_pos;
	glm::vec3 v0v2 = m_v2.m_pos - m_v0.m_pos;
	glm::vec3 pVec = glm::cross(ray->getDirection(), v0v2);
	float det = glm::dot(v0v1, pVec);

	//Culling
	if (det < kEpsilon)
		return false;

	if(fabs(det) < kEpsilon)
		return false;
	float invDet = 1 / det;

	glm::vec3 tvec = ray->getOrigin() - m_v0.m_pos;
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

	return (t > 0) ? true : false;
}

void Triangle::getSurfaceData(const glm::vec3 & pHit, glm::vec3 & nHit, glm::vec2 & tex) const
{
}

void Triangle::getSurface(const glm::vec3& hitPoint, const glm::vec3& viewDirection,
									const uint64_t& index, const glm::vec2& uv, glm::vec3& hitNormal,
									glm::vec2& hitTextureCoordinate) const
{
	hitNormal = (1 - uv.x - uv.y) * m_v0.m_normal + uv.x * m_v0.m_normal + uv.y * m_v2.m_normal;
	
	hitNormal = glm::normalize(hitNormal);

	hitTextureCoordinate = (1 - uv.x - uv.y) * m_v0.m_uv + uv.x * m_v1.m_uv + uv.y * m_v2.m_uv;
}
