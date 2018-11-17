#include "Camera.h"

Camera::Camera(const float & fieldOfView, const float & aspectRatio, 
			   const glm::vec3 & up, const glm::vec3 target)
	: m_fov(fieldOfView), m_aspectRatio(aspectRatio), m_up(up), m_target(target)
{
	m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	m_rotation = glm::lookAt(m_pos, m_target, m_up);
}
