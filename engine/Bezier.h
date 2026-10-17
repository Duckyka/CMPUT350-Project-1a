#ifndef BEZIER_H
#define BEZIER_H

#include "MathUtil.h"
#include <vector>

using Point2D = CMPUT350::Point2D;

class Bezier
{
public:
	Bezier(const std::vector<Point2D> &pts);
	
	Point2D GetPoint(float t) const;
	Point2D GetSlope(float t) const;
	
	static Point2D GetPoint(const std::vector<Point2D> &pts, float t);
	static Point2D GetSlope(const std::vector<Point2D> &pts, float t);
    
    std::vector<Point2D> GetCurve();
private:
    std::vector<Point2D> mPoints;
    std::vector<Point2D> mCurve;
    float mResolution = 60.0f;
};

#endif //BEZIER_H