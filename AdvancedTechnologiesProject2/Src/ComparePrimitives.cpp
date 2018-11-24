#include "ComparePrimitives.h"

#include "Geometry.h"

ComparePrimitives::ComparePrimitives(Axis axis)
	: m_axis(axis)
{
}

bool ComparePrimitives::operator()(std::shared_ptr<Geometry> a, std::shared_ptr<Geometry> b)
{
	float ca = a->getPos()[m_axis];
	float cb = b->getPos()[m_axis];
								  
	return ca < cb;
}
