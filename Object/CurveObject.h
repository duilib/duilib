#ifndef CurveObject_h__
#define CurveObject_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	CurveObject.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2015-05-22 16:13:53
// 说明			:	曲线
/////////////////////////////////////////////////////////////*/
#pragma once
class Point2D;

namespace DuiLib
{

	// 2D坐标
class UILIB_API Point2D
{
public:
	Point2D(float x,float y);
	Point2D(const Point2D& rhs);
	void Add(Point2D& other);
	Point2D Scale(float t);

	float x;
	float y;
};

// 2D坐标集合
class UILIB_API CCurveObject
{
	friend class BezierAlgorithm;
public:
	CCurveObject(void);
	~CCurveObject(void);

	void AddPoint(float x, float y);
	void Reset();

protected:
	Point2DVector m_vec;
};

}
#endif // CurveObject_h__
