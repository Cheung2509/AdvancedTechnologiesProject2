#pragma once

#include "Object.h"
#include "AABB.h"

#include <memory>

class Ray;

enum MaterialType
{
	DIFFUSE_AND_GLOSSY,
	REFLECTION_AND_REFRACTION,
	REFLECTION
};

class Geometry : public Object
{
public:
	Geometry() = default;
	Geometry(const glm::vec3& col, MaterialType type = DIFFUSE_AND_GLOSSY, float ior = 1.3f, float kd = 0.8f, float ks = 0.2, 
		  float specular = 25);
	virtual ~Geometry() = default;

	virtual bool intersect(Ray* ray, std::uint64_t &, glm::vec2 &) = 0;
	virtual void getSurfaceData(const glm::vec3& pHit, glm::vec3& nHit, glm::vec2& tex) const = 0;

	bool solveQuadratic(const float& a, const float& b, float& c, float& x0, float& x1);

	const glm::vec3& getDiffuseColour() const { return m_diffuseColour; }
	const MaterialType& getMaterialType() const { return m_matType; }
	const float& getIOR() const { return m_ior; }
	const float& getKD()  const { return m_kd; }
	const float& getKS()  const { return m_ks; }
	const float& getSpecularExponent() const { return m_specularExponent; }
	const AABB& getBox() const { return m_boundingBox; }

	void setIOR(const float& ior) { m_ior = ior; }
	void setKD(const float& kd) { m_kd = kd; }
	void setKS(const float& ks) { m_ks = ks; }
	void setSpecularExponent(const float& se) { m_specularExponent = se; }
	void setDiffuseColour(const glm::vec3& col) { m_diffuseColour = col; }
	void setMaterialType(const MaterialType& type) { m_matType = type; }


	virtual void getSurfaceProperties(const glm::vec3&, const glm::vec3&, const uint64_t&, const glm::vec2&, glm::vec3&, glm::vec2&) const = 0;
	virtual glm::vec3 evalDiffuseColour(const glm::vec2&) const { return m_diffuseColour; }

protected:
	glm::vec3 m_diffuseColour;

	MaterialType m_matType;

	float m_ior;
	float m_kd;
	float m_ks;
	float m_specularExponent;

	AABB m_boundingBox;
};