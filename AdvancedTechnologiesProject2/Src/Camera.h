#pragma once

#include "Object.h"


class Camera : public Object
{
public:
	Camera() = default;
	Camera(const float& fieldOfView, const float& aspectRatio, 
		   const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f), 
		   const glm::vec3 target = glm::vec3(0, 0, 0), const float& nearPlane = 0.1f,
		   const float& farPlane = 100);
	~Camera() = default;

	const float& getFOV() const { return m_fov; }
	const float& getAspectRatio() const { return m_aspectRatio; }
	const glm::mat4& getProjection() const { return m_projectionMatrix; }

private:
	
	float m_fov;
	float m_aspectRatio;
	float m_nearPlaneDistance;
	float m_farPlaneDistance;

	glm::vec3 m_target;
	glm::vec3 m_up;

	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;
};