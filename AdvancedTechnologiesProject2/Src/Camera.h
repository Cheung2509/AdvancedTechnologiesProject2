#pragma once

#include "Actor.h"


class Camera : public Actor
{
public:
	Camera() = default;
	Camera(const float& fieldOfView, const float& aspectRatio, 
		   const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f), 
		   const glm::vec3 target = glm::vec3(0, 0, 0));
	~Camera() = default;

	const float& getFOV() const { return m_fov; }
	const float& getAspectRatio() const { return m_aspectRatio; }

private:
	
	float m_fov;
	float m_aspectRatio;

	glm::vec3 m_target;
	glm::vec3 m_up;
};