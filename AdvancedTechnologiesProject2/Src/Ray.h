#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Image.h"

constexpr float kInfinity = std::numeric_limits<float>::max();

class Geometry;
class Light;
class BVH;

class Ray
{
public:
	Ray() = default;
	Ray(const glm::vec3& rayOrig, const glm::vec3& rayDir, unsigned int depth);
	~Ray() = default;

	const bool trace(const std::vector<std::shared_ptr<Geometry>> &objects);
	const bool trace(std::shared_ptr<BVH> &bvh);

	const float mix(const float& a, const float& b, const float& mix);
	const glm::vec3 mix(const glm::vec3& a, const glm::vec3& b, const float & mixValue);

	glm::vec3 castRay(const std::vector<std::shared_ptr<Geometry>>& shapes,
					  const std::vector<std::shared_ptr<Light>>& lights, ImageData data,
					  bool test = false);

	glm::vec3 castRay(std::shared_ptr<BVH> bvh, const std::vector<std::shared_ptr<Light>>& lights, ImageData data,
					  bool test = false);

	glm::vec3 reflect(const glm::vec3& i, const glm::vec3& n);
	glm::vec3 refract(const glm::vec3& i, const glm::vec3& n, const float& ior);
	void fresnel(const glm::vec3& i, glm::vec3& n, const float &ior, float& kr);


	const glm::vec3& getOrigin() const { return m_rayOrig; }
	const glm::vec3& getDirection() const { return m_rayDirection; }
	const glm::vec3& getInvDirection() const { return m_invDirection; }
	const int& getSign(const int& i) const { return m_sign[i]; }
	const float& getClosestHit() const { return m_closestHit; }
	const glm::vec2& getUV() const { return m_uv; }

	void setHit(const float& hit) { m_closestHit = hit; }
private:
	glm::vec3 m_rayOrig;
	glm::vec3 m_rayDirection;
	glm::vec3 m_invDirection;
	glm::vec2 m_uv = glm::vec2(0);
	
	float m_closestHit = kInfinity;
	unsigned int m_depth;
	std::uint64_t m_index = 0;

	Geometry* m_hitObject = nullptr;

	int m_sign[3];
};