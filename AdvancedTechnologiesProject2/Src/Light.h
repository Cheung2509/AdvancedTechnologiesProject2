#pragma once

#include "Actor.h"

class Light : public Actor
{
public:
	Light() {};
	~Light() {};

	const float& getIntensity() const { return m_intensity; }
	void setIntensity(const float& intensity) { m_intensity = intensity; }
private:
	float m_intensity = 1;

};