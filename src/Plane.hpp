#pragma once

#include "common.view.h"

/**
 * @brief How angles are stored.
 */
typedef double AngleType;
namespace AngleUnit
{
enum AngleUnit
{
	Cycle,
	Quadrant,
	Degree,
	MinuteOfArc,
	SecondOfArc,
	Radian
};
}

/**
 * @brief Represents a plane in a mathematical sense.
 */
struct Plane
{
public:

	float xAngle;
	float yAngle;
	float zAngle;
	AngleUnit::AngleUnit unit;

public:

	Plane();
	Plane(float xAngle, float yAngle, float zAngle);
	Plane(float xAngle, float yAngle, float zAngle, AngleUnit::AngleUnit unit);

public:

	void Change(float xAngleChange, float yAngleChange, float zAngleChange);

};
