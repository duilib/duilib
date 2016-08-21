#include "stdafx.h"
#include "CurveObject.h"

namespace DuiLib
{

//////////////////////////////////////////////////////////////////////////
// CCurveObject
CCurveObject::CCurveObject(void)
{

}


CCurveObject::~CCurveObject(void)
{
	m_vec.clear();
}

void CCurveObject::AddPoint(float x, float y)
{
	Point2D pt(x,y);
	m_vec.push_back(pt);
}

void CCurveObject::Reset()
{
	m_vec.clear();
}

//////////////////////////////////////////////////////////////////////////
// Point2D
Point2D::Point2D(float x,float y)
{
	this->x = x;
	this->y = y;
}

Point2D::Point2D(const Point2D& rhs )
{
	this->x = rhs.x;
	this->y = rhs.y;
}

void Point2D::Add(Point2D& other)
{
	x+=other.x;
	y+=other.y;
}

Point2D Point2D::Scale(float t)
{
	return Point2D(x*t,y*t);
}

}