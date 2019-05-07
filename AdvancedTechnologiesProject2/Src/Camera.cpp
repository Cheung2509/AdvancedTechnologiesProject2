#include "Camera.h"

Camera::Camera(const float & fieldOfView, const float & aspectRatio, 
			   const glm::vec3 & up, const glm::vec3 target, const float& nearPlane, const float& farPlane)
	: m_fov(fieldOfView), m_aspectRatio(aspectRatio), m_up(up), m_target(target), m_nearPlaneDistance(nearPlane),
	m_farPlaneDistance(farPlane)
{
	m_pos = glm::vec3(0.0f, 0.0f, 5.0f);
	
	m_rotation = glm::lookAt(m_pos, m_target, m_up);

	m_projectionMatrix = glm::perspective(fieldOfView * glm::pi<float>() / 180.0f, m_aspectRatio,
										  m_nearPlaneDistance, m_farPlaneDistance) *  glm::lookAt(m_pos, m_target, m_up);
	m_viewMatrix = glm::lookAt(m_pos, m_target, m_up);
}
