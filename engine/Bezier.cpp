#include "Bezier.h"
#include <vector>
#include <SFML/Graphics.hpp>


Bezier::Bezier(const std::vector<Point2D> &pts)
{
    if (pts.size() <= 4)
    {
        mPoints = pts;
        mCurve = std::vector<Point2D>(mResolution);
        float t = 0.0f;
        for (int i = 0; i < mResolution; i++)
        {
            t = i / mResolution;
            Point2D bezierPoint = GetPoint(t);
            mCurve[i].x = bezierPoint.x;
            mCurve[i].y = bezierPoint.y;
        }
    }
    else
    {
        int ExtraCurveNum = (pts.size() - 4) / 3;
        mPoints = pts;
        mCurve = std::vector<Point2D>(mResolution * (ExtraCurveNum + 1));
        float t = 0.0f;
        for (int i = 0; i < ExtraCurveNum; i++)
        {
            std::vector<Point2D> tempCurvePoints;
            tempCurvePoints.push_back(pts.at(i * 3 + 3));
            tempCurvePoints.push_back(pts.at(i * 3 + 4));
            tempCurvePoints.push_back(pts.at(i * 3 + 5));
            tempCurvePoints.push_back(pts.at(i * 3 + 6));
            for (int j = 0; j < mResolution; j++)
            {
                t = j / mResolution;
                Point2D bezierPoint = GetPoint(t);
                mCurve[j + (i * mResolution)].x = bezierPoint.x;
                mCurve[j + (i * mResolution)].y = bezierPoint.y;
            }
            tempCurvePoints.clear();
        }
    }
}
	
Point2D Bezier::GetPoint(float t) const
{
    Point2D bezierCurve;
    bezierCurve.x = (std::pow(1-t, 3) * mPoints.at(0).x + t*mPoints.at(1).x*(3*std::pow(1-t, 2)) + mPoints.at(2).x*(3*(1-t)*std::pow(t, 2)) + mPoints.at(3).x*std::pow(t, 3));
    bezierCurve.y = (std::pow(1-t, 3) * mPoints.at(0).y + t*mPoints.at(1).y*(3*std::pow(1-t, 2)) + mPoints.at(2).y*(3*(1-t)*std::pow(t, 2)) + mPoints.at(3).y*std::pow(t, 3));
    return bezierCurve;
}
Point2D Bezier::GetSlope(float t) const
{
    Point2D bezierSlope;
    bezierSlope.x = (3*std::pow(1-t, 2)*(mPoints.at(1).x - mPoints.at(0).x) + 6*(1 - t) * t * (mPoints.at(2).x - mPoints.at(1).x) + 3*std::pow(t, 2)*(mPoints.at(3).x - mPoints.at(2).x));
    bezierSlope.y = (3*std::pow(1-t, 2)*(mPoints.at(1).y - mPoints.at(0).y) + 6*(1 - t) * t * (mPoints.at(2).y - mPoints.at(1).y) + 3*std::pow(t, 2)*(mPoints.at(3).y - mPoints.at(2).y));
    return bezierSlope;
}

static Point2D GetPoint(const std::vector<Point2D> &pts, float t)
{
    Point2D bezierCurve;
    bezierCurve.x = (std::pow(1-t, 3) * pts.at(0).x + t*pts.at(1).x*(3*std::pow(1-t, 2)) + pts.at(2).x*(3*(1-t)*std::pow(t, 2)) + pts.at(3).x*std::pow(t, 3));
    bezierCurve.y = (std::pow(1-t, 3) * pts.at(0).y + t*pts.at(1).y*(3*std::pow(1-t, 2)) + pts.at(2).y*(3*(1-t)*std::pow(t, 2)) + pts.at(3).y*std::pow(t, 3));
    return bezierCurve;
}
static Point2D GetSlope(const std::vector<Point2D> &pts, float t)
{
    Point2D bezierSlope;
    bezierSlope.x = (3*std::pow(1-t, 2)*(pts.at(1).x - pts.at(0).x) + 6*(1 - t) * t * (pts.at(2).x - pts.at(1).x) + 3*std::pow(t, 2)*(pts.at(3).x - pts.at(2).x));
    bezierSlope.y = (3*std::pow(1-t, 2)*(pts.at(1).y - pts.at(0).y) + 6*(1 - t) * t * (pts.at(2).y - pts.at(1).y) + 3*std::pow(t, 2)*(pts.at(3).y - pts.at(2).y));
    return bezierSlope;
}

std::vector<Point2D> Bezier::GetCurve()
{
    return mCurve;
}