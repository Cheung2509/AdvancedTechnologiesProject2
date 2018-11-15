#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Actor
{
public:
	Actor() = default;
	virtual ~Actor() = default;

	const glm::vec3& getPos() const { return m_pos; }
	void setPos(const glm::vec3& pos) { m_pos = pos; }

	const glm::quat& getRotation() const { return m_rotation; }
	void setRotation(const glm::quat& rot) { m_rotation = rot; }

protected:
	glm::vec3 m_pos;
	glm::quat m_rotation;
};