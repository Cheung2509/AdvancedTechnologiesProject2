#pragma once

#include "Object.h"
#include "AABB.h"

#include <memory>

class Ray;

enum MaterialType
{
	DIFFUSE,
	REFLECTION_AND_REFRACTION,
	REFLECTION,
	PHONG
};

enum GeometryType
{
	MESH, 
	PRIMITIVE
};

class Geometry : public Object
{
public:
	Geometry() = default;
	Geometry(const glm::vec3& col, GeometryType gType, MaterialType type = DIFFUSE, float ior = 1.3f, float kd = 0.8f, float ks = 0.2, 
		  float specular = 25);
	virtual ~Geometry() = default;

	virtual bool intersect(Ray* ray, std::uint64_t &, glm::vec2 &, float&) = 0;
	virtual void getSurfaceData(const glm::vec3& pHit, glm::vec3& nHit, glm::vec2& tex) const = 0;

	bool solveQuadratic(const float& a, const float& b, float& c, float& x0, float& x1);

	const glm::vec3& getDiffuseColour() const { return m_diffuseColour; }
	const MaterialType& getMaterialType() const { return m_matType; }
	const GeometryType& getGeometryType() const { return m_geometryType; }

	const float& getIOR() const { return m_ior; }
	const float& getKD()  const { return m_kd; }
	const float& getKS()  const { return m_ks; }
	const float& getSpecularExponent() const { return m_specularExponent; }
	const glm::vec3& getAlbedo() const { return m_albedo; }
	const AABB& getBox() const { return m_boundingBox; }

	void setIOR(const float& ior) { m_ior = ior; }
	void setKD(const float& kd) { m_kd = kd; }
	void setKS(const float& ks) { m_ks = ks; }
	void setSpecularExponent(const float& se) { m_specularExponent = se; }
	void setDiffuseColour(const glm::vec3& col) { m_diffuseColour = col; }
	void setMaterialType(const MaterialType& type) { m_matType = type; }


	virtual void getSurface(const glm::vec3&, const glm::vec3&, const uint64_t&, const glm::vec2&, glm::vec3&, glm::vec2&) const = 0;
	virtual glm::vec3 evalDiffuseColour(const glm::vec2&) const { return m_diffuseColour; }
protected:
	glm::vec3 m_diffuseColour;

	MaterialType m_matType;
	GeometryType m_geometryType;

	float m_ior;
	float m_kd; //diffuse weight;
	float m_ks;// specular weight
	float m_specularExponent; //Specular exponent
	glm::vec3 m_albedo = glm::vec3(0.18);

	AABB m_boundingBox;
};