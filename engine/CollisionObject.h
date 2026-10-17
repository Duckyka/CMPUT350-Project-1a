#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H

#include <memory>
#include <vector>

#include "GraphicsObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

enum class ShapeType {
    kLine, kCircle, kRect
};
union ShapeUnion {
    ShapeUnion(Line l) : line(l) {}
    ShapeUnion(Circle c) : circle(c) {}
    ShapeUnion(Rect r) : rect(r) {}
    Line line;
    Circle circle;
    Rect rect; // Axis-aligned rectangle
};
struct Shape {
    Shape(const Line &l) : shape(l), t(ShapeType::kLine) {}
    Shape(const Circle &c) : shape(c), t(ShapeType::kCircle) {}
    Shape(const Rect &r) : shape(r), t(ShapeType::kRect) {}
    ShapeUnion shape;
    ShapeType t;
};

// Shape Collisions helpers
bool Overlaps(const Rect &a, const Rect &b);
bool LineHitsLine(const Line &l, const Line &o, float &t);
bool LineHitsCircle(const Line &l, const Circle &c, float &t);
bool LineHitsRect(const Line &l, const Rect &r, float &t);
bool CircleHitsCircle(const Circle &a, const Circle &b);
bool CircleHitsRect(const Circle &c, const Rect &r);
Shape ToWorld(const Shape &s, Point2D location, float rotation);
bool ShapesCollide(const Shape &a, const Shape &b, bool &hasPoint, Point2D &worldPoint, float &t);

class CollisionObject : public GraphicsObject {
public:
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) = 0;
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject> &obj, const Point2D &collisionPoint)
	{ CollisionEnter(obj); }
    // Box around all of the object's shapes, in local space.
    // The engine moves and turns it to the screen.
    virtual const Rect &GetBounds() = 0;
    // Return the internal shapes in the object. Shapes can be:
    // lines, points/circles, or axis-aligned rectangles
    virtual const std::vector<Shape> &GetShapes() = 0;
};

}  // namespace CMPUT350

#endif
