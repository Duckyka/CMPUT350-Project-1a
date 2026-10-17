#include "Bezier.h"
#include <vector>

namespace {
    /*
    Find which segment t falls in.
    Input:  numSegments - how many cubic segments the curve has
            t - global time, from 0 to numSegments
    Output: the segment index. t is changed to the local time (0 to 1) in that segment.
    */
    int FindSegment(int numSegments, float &t)
    {
        // Keep t inside the curve
        if (t < 0.0f) {
            t = 0.0f;
        }
        if (t > numSegments) {
            t = static_cast<float>(numSegments);
        }

        int segment = static_cast<int>(t);
        // t at the very end belongs to the last segment
        if (segment >= numSegments) {
            segment = numSegments - 1;
        }
        t -= segment;
        return segment;
    }
}

/*
Store the control points. Needs 3k + 1 points for k segments.
*/
Bezier::Bezier(const std::vector<Point2D> &pts)
    : mPoints(pts)
{
}

/*
Output: the point on this curve at time t (0 <= t <= number of segments)
*/
Point2D Bezier::GetPoint(float t) const
{
    return GetPoint(mPoints, t);
}

/*
Output: the slope (direction of travel) on this curve at time t
*/
Point2D Bezier::GetSlope(float t) const
{
    return GetSlope(mPoints, t);
}

/*
Output: the number of cubic segments in pts (0 if there are fewer than 4 points)
*/
int Bezier::GetNumSegments(const std::vector<Point2D> &pts)
{
    if (pts.size() < 4) {
        return 0;
    }
    return (static_cast<int>(pts.size()) - 1) / 3;
}

/*
Output: the point on the curve made by pts at time t (0 <= t <= number of segments)
B(t) = (1-t)^3 P0 + 3(1-t)^2 t P1 + 3(1-t) t^2 P2 + t^3 P3
*/
Point2D Bezier::GetPoint(const std::vector<Point2D> &pts, float t)
{
    int numSegments = GetNumSegments(pts);
    if (numSegments == 0) {
        // Not enough points for a curve
        return pts.empty() ? Point2D(0.0f, 0.0f) : pts.front();
    }

    // Control points for the segment t is in
    int base = FindSegment(numSegments, t) * 3;
    const Point2D &p0 = pts[base];
    const Point2D &p1 = pts[base + 1];
    const Point2D &p2 = pts[base + 2];
    const Point2D &p3 = pts[base + 3];

    // Weights for each control point
    float u = 1.0f - t;
    float b0 = u * u * u;
    float b1 = 3.0f * u * u * t;
    float b2 = 3.0f * u * t * t;
    float b3 = t * t * t;

    return Point2D(b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x,
                   b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y);
}

/*
Output: the slope (derivative) of the curve made by pts at time t
B'(t) = 3(1-t)^2 (P1 - P0) + 6(1-t) t (P2 - P1) + 3 t^2 (P3 - P2)
*/
Point2D Bezier::GetSlope(const std::vector<Point2D> &pts, float t)
{
    int numSegments = GetNumSegments(pts);
    if (numSegments == 0) {
        // Not enough points for a curve
        return Point2D(0.0f, 0.0f);
    }

    // Control points for the segment t is in
    int base = FindSegment(numSegments, t) * 3;
    const Point2D &p0 = pts[base];
    const Point2D &p1 = pts[base + 1];
    const Point2D &p2 = pts[base + 2];
    const Point2D &p3 = pts[base + 3];

    // Weights for each pair of control points
    float u = 1.0f - t;
    float d0 = 3.0f * u * u;
    float d1 = 6.0f * u * t;
    float d2 = 3.0f * t * t;

    return Point2D(d0 * (p1.x - p0.x) + d1 * (p2.x - p1.x) + d2 * (p3.x - p2.x),
                   d0 * (p1.y - p0.y) + d1 * (p2.y - p1.y) + d2 * (p3.y - p2.y));
}
