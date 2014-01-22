#pragma once

/*!
 * \brief 2-dimensional point.
 */
struct Point2D
{
public:
	float x;
	float y;

public:
	Point2D();
	Point2D(float x, float y);
	virtual void reset();

};
