#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Actor
{
public:
	Actor();
	virtual ~Actor() = default;

	const glm::vec3& getPos() const { return m_pos; }
	void setPos(const glm::vec3& pos) { m_pos = pos; }

	const glm::quat& getRotation() const { return m_rotation; }
	void setRotation(const glm::quat& rot) { m_rotation = rot; }

	const glm::mat4& getWorld() const { return m_worldMatrix; }

protected:
	glm::vec3 m_pos;
	glm::quat m_rotation;

	glm::mat4 m_worldMatrix = glm::mat4(1.0f);;
};