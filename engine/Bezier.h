#ifndef BEZIER_H
#define BEZIER_H

#include "MathUtil.h"
#include <vector>

using Point2D = CMPUT350::Point2D;

/*
Cubic bezier curves with 4 control points per segment.
Segments share end points, so k segments use 3k + 1 points and t goes from 0 to k.
Can be used through a Bezier object or through the static functions.
*/
class Bezier
{
public:
	Bezier(const std::vector<Point2D> &pts);

	Point2D GetPoint(float t) const;
	Point2D GetSlope(float t) const;

	static Point2D GetPoint(const std::vector<Point2D> &pts, float t);
	static Point2D GetSlope(const std::vector<Point2D> &pts, float t);

	// Number of cubic segments, so t can go from 0 to GetNumSegments()
	static int GetNumSegments(const std::vector<Point2D> &pts);

private:
	std::vector<Point2D> mPoints;
};

#endif //BEZIER_H
