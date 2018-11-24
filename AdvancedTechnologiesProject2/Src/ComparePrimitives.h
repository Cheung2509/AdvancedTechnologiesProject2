#pragma once

#include <memory>

class Geometry;

enum Axis
{
	X = 0,
	Y = 1,
	Z = 2
};

class ComparePrimitives
{
public:
	ComparePrimitives(Axis axis);

	bool operator() (std::shared_ptr<Geometry> a, std::shared_ptr<Geometry> b);


private:
	Axis m_axis;
};