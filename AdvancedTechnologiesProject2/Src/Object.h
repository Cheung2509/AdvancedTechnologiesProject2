#pragma once

#include <glm/glm.hpp>
#include "glm/gtx/transform.hpp"
#include <glm/gtc/quaternion.hpp>

class Object
{
public:
	Object() = default;
	virtual ~Object() = default;

	const glm::vec3& getPos() const { return m_pos; }
	void setPos(const glm::vec3& pos) { m_pos = pos; }

	const glm::quat& getRotation() const { return m_rotation; }
	void setRotation(const glm::quat& rot) { m_rotation = rot; }

	const glm::mat4 getWorld() const 
	{
		glm::mat4 transMat = glm::translate(glm::mat4(1.0f), m_pos);
		glm::mat4 scaleMat = glm::scale(m_scale);
		return transMat * glm::mat4_cast(m_rotation) * scaleMat; 
	}
protected:
	glm::vec3 m_pos;
	glm::vec3 m_scale;
	glm::quat m_rotation;
};