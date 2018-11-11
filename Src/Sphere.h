#pragma once

#include <SFML/System.hpp>

class Sphere
{
public:
	Sphere() = default;
	~Sphere() = default;

	
private:
	float m_radius;
	sf::Vector3f m_origin;
};