#include "CollisionObject.h"
#include <cmath>
#include <algorithm>

namespace CMPUT350 {

/*
Checks if two axis-aligned rectangles overlap
*/
bool Overlaps(const Rect &a, const Rect &b) {
    // If one rect is fully left, right, above, or below the other, they can't overlap
    bool separated = 
            (a.topLeft.x + a.width) < b.topLeft.x ||     // a is left of b
            a.topLeft.x > (b.topLeft.x + b.width) ||    // a is right of b
            (a.topLeft.y + a.height) < b.topLeft.y ||   // a is above b
            a.topLeft.y > (b.topLeft.y + b.height);     // a is below b

    return !separated;
}

/*
Checks if directed line l crosses line o
Output: true on a hit, t = fraction along l where it crosses (0..1)
*/
bool LineHitsLine(const Line &l, const Line &o, float &t) {
    Point2D lineDirection = l.p2 - l.p1;
    Point2D otherDirection = o.p2 - o.p1;
    float directionCross = Point2D::Cross(lineDirection,otherDirection);
    if (directionCross == 0) {
        return false; // parellel
    }

    Point2D startToOtherStart = o.p1 - l.p1;
    float fractionOnLine = Point2D::Cross(startToOtherStart, otherDirection) / directionCross;
    float fractionOnOther = Point2D::Cross(startToOtherStart, lineDirection) / directionCross;
    if (fractionOnLine < 0 || fractionOnLine > 1 || fractionOnOther < 0 || fractionOnOther > 1) {
        return false;
    }
    t = fractionOnLine;
    return true;

}

/*
Checks if directed line l touches circle c.
Output: true on a hit, t = fraction along l of the first point inside the circle
*/
bool LineHitsCircle(const Line &l, const Circle &c, float &t) {
    Point2D centerToStart = l.p1 - c.center;
    float radiusSquared = c.radius * c.radius;

    // The line starts inside the circle, so the first hit is the start point
    if (centerToStart * centerToStart <= radiusSquared) {
        t = 0;
        return true;
    }

    // Points on the line: l.p1 + fraction * lineDirection
    // Setting the distance to the center equal to the radius gives a quadratic
    Point2D lineDirection = l.p2 - l.p1;
    float quadA = lineDirection * lineDirection;
    if (quadA == 0) {
        return false; // the line has no length, and its single point is outside
    }

    float quadB = 2 * (centerToStart * lineDirection);
    float quadC = centerToStart * centerToStart - radiusSquared;
    float discriminant = quadB * quadB - 4 * quadA * quadC;
    if (discriminant < 0) {
        return false; // the line never reaches the circle
    }

    // The smaller root is where the line enters the circle
    float entryFraction = (-quadB - std::sqrt(discriminant)) / (2 * quadA);
    if (entryFraction < 0 || entryFraction > 1) {
        return false; // the circle is behind the start or past the end
    }

    t = entryFraction;
    return true;

}

/*
Checks if a directed line touches an eaxis-aligned rectangle
Output: true on a hit, fractionAlongLine = where the line first enters the rect
*/
bool LineHitsRect(const Line &l, const Rect &r, float &t) {
    // The line starts inside the rect, so the first hit is the start point
    if (r.IsInside(l.p1)) {
        t = 0;
        return true;
    }

    Point2D topLeft = r.topLeft;
    Point2D topRight(topLeft.x + r.width, topLeft.y);
    Point2D bottomLeft(topLeft.x, topLeft.y + r.height);
    Point2D bottomRight(topLeft.x + r.width, topLeft.y + r.height);

    Line rectEdges[4] = {
    Line(topLeft, topRight),
    Line(topRight, bottomRight),
    Line(bottomRight, bottomLeft),
    Line(bottomLeft, topLeft)
    };

    // Check each edge and keep the crossing closes to the start of the line
    bool foundHit = false;
    float closestFraction = 2; // anything above 1 means "no hit yet"
    for (const Line &edge : rectEdges) {
        float edgeFraction;
        if (LineHitsLine(l, edge, edgeFraction) && edgeFraction < closestFraction) {
            closestFraction = edgeFraction;
            foundHit = true;
        }
    }

    if (foundHit) {
        t = closestFraction;
    }
    return foundHit;
}

/*
Checks if two circles touch
*/
bool CircleHitsCircle(const Circle &firstCircle, const Circle &secondCircle) {
    Point2D centerToCenter = firstCircle.center - secondCircle.center;
    float radiusSum = firstCircle.radius + secondCircle.radius;
    // Compared squared distances to avoid a square root
    return centerToCenter * centerToCenter <= radiusSum * radiusSum;
}

/*
Checks if a circle touches an axis-aligned rectangle.
Finds the point on the rect closest to the circle's center, then checks if it's within the radius.
*/
bool CircleHitsRect(const Circle &circle, const Rect &rect)
{
    float closestX = std::clamp(circle.center.x, rect.topLeft.x, rect.topLeft.x + rect.width);
    float closestY = std::clamp(circle.center.y, rect.topLeft.y, rect.topLeft.y + rect.height);

    Point2D centerToClosest = circle.center - Point2D(closestX, closestY);
    return centerToClosest * centerToClosest <= circle.radius * circle.radius;
}

/*
Moves a shape from the object's local space to screen space.
Same math as DrawContext::Transform: rotate around (0,0), then add the object's location.
Rects must stay axis-aligned, so a rotated rect becomes the box around its 4 rotated corners.
*/
Shape ToWorld(const Shape &localShape, Point2D objectLocation, float objectRotation) {
    float cosRotation = std::cos(objectRotation);
    float sinRotation = std::sin(objectRotation);

    // Rotates a local point and moves it to the object's location on screen
    auto toScreen = [&](Point2D localPoint) {
        return Point2D(localPoint.x * cosRotation - localPoint.y * sinRotation + objectLocation.x,
                        localPoint.x * sinRotation + localPoint.y * cosRotation + objectLocation.y);
    };

    switch (localShape.t) {
        case CMPUT350::ShapeType::kLine:
        {
            const Line &localLine = localShape.shape.line;
            return Shape(Line(toScreen(localLine.p1), toScreen(localLine.p2)));
        }

        case CMPUT350::ShapeType::kCircle:
        {
            // Rotating doesn't change a circle's radius, only where its center is
            const Circle &localCircle = localShape.shape.circle;
            return Shape(Circle(toScreen(localCircle.center), localCircle.radius));
        }
        case CMPUT350::ShapeType::kRect:
        default:
        {
            const Rect &localRect = localShape.shape.rect;
            Point2D topLeft = localRect.topLeft;
            Point2D bottomRight(topLeft.x + localRect.width, topLeft.y + localRect.height);
            Point2D topRight(bottomRight.x, topLeft.y);
            Point2D bottomLeft(topLeft.x, bottomRight.y);

            // Any corner could end up furthest out after rotating, so include all 4
            Rect screenRect(toScreen(topLeft), toScreen(bottomRight));
            screenRect |= toScreen(topRight);
            screenRect |= toScreen(bottomLeft);
            return Shape(screenRect);
        }
    }
}

/*
Tests two screen-space shapes against each other.
Output: true on a hit.
        If a line is involved: hasCollisionPoint = true, collisionPoint is the first point
        along that line that touches the other shape, and fractionAlongLine says how far along it is.
        Otherwise hasCollisionPoint = false (circles/rects don't have a single collision point).
*/
bool ShapesCollide(const Shape &firstShape, const Shape &secondShape,
                   bool &hasCollisionPoint, Point2D &collisionPoint, float &fractionAlongLine)
{
    hasCollisionPoint = false;

    // If either shape is a line, treat that one as the directed line
    const Shape *lineShape = nullptr;
    const Shape *otherShape = nullptr;
    if (firstShape.t == ShapeType::kLine)
    {
        lineShape = &firstShape;
        otherShape = &secondShape;
    }
    else if (secondShape.t == ShapeType::kLine)
    {
        lineShape = &secondShape;
        otherShape = &firstShape;
    }

    if (lineShape != nullptr)
    {
        const Line &line = lineShape->shape.line;
        bool isHit = false;
        switch (otherShape->t)
        {
        case ShapeType::kLine:
            isHit = LineHitsLine(line, otherShape->shape.line, fractionAlongLine);
            break;
        case ShapeType::kCircle:
            isHit = LineHitsCircle(line, otherShape->shape.circle, fractionAlongLine);
            break;
        case ShapeType::kRect:
            isHit = LineHitsRect(line, otherShape->shape.rect, fractionAlongLine);
            break;
        }

        if (isHit)
        {
            hasCollisionPoint = true;
            collisionPoint = line.p1 + (line.p2 - line.p1) * fractionAlongLine;
        }
        return isHit;
    }

    // No lines, so check circles and rects
    if (firstShape.t == ShapeType::kCircle && secondShape.t == ShapeType::kCircle)
        return CircleHitsCircle(firstShape.shape.circle, secondShape.shape.circle);

    if (firstShape.t == ShapeType::kRect && secondShape.t == ShapeType::kRect)
        return Overlaps(firstShape.shape.rect, secondShape.shape.rect);

    if (firstShape.t == ShapeType::kCircle)
        return CircleHitsRect(firstShape.shape.circle, secondShape.shape.rect);

    return CircleHitsRect(secondShape.shape.circle, firstShape.shape.rect);
}

}