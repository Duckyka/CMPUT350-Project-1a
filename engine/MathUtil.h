#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <algorithm>
#include <cmath>
#include <iostream>

namespace CMPUT350 {

struct Point2D {
    float x, y;
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}
    double Distance(const Point2D &other) const {
        // TODO: write this code
        float deltaX = x - other.x;
        float deltaY = y - other.y;
        float distance = sqrt(((deltaX * deltaX) + (deltaY * deltaY)));
        return distance;
    }
    Point2D operator+(const Point2D &other) const {
        // TODO: write this code
        Point2D temp(0, 0);
        temp.x = x + other.x;
        temp.y = y + other.y;
        return temp;
    }
    Point2D operator+(const float &other) const {
        // TODO: write this code
        Point2D temp(0, 0);
        temp.x = x + other;
        temp.y = y + other;
        return temp;
    }
    Point2D operator-(const Point2D &other) const {
        // TODO: write this code
        Point2D temp(0, 0);
        temp.x = x - other.x;
        temp.y = y - other.y;
        return temp;
    }
    Point2D operator-(const float &other) const {
        // TODO: write this code
        Point2D temp(0, 0);
        temp.x = x - other;
        temp.y = y - other;
        return temp;
    }
    Point2D operator*(const float &scalar) const {
        // TODO: write this code
        Point2D temp(0, 0);
        temp.x = x * scalar;
        temp.y = y * scalar;
        return temp;
    }
    Point2D &operator+=(const float &scalar) {
        // TODO: write this code
        x = x + scalar;
        y = y + scalar;
        return *this;
    }
    Point2D &operator+=(const Point2D &other) {
        // TODO: write this code
        x = x + other.x;
        y = y + other.y;
        return *this;
    }
    Point2D &operator-=(const Point2D &other) {
        // TODO: write this code
        x = x - other.x;
        y = y - other.y;
        return *this;
    }
    bool operator==(const Point2D &other) const {
        // TODO: write this code
        if (x == other.x && y == other.y)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    Point2D &operator*=(const int &scalar) {
        // TODO: write this code
        if (scalar == 0)
        {
            x = 0;
            y = 0;
            return *this;
        }
        else
        {
            x = x * scalar;
            y = y * scalar;
            return *this;
        }
    }
    Point2D &operator/=(const int &scalar) {                //Emit a warning for division by 0
        // TODO: write this code
        
        x = x / scalar;
        y = y / scalar;
        return *this;
    }
    float operator*(const Point2D &other) const {
        // TODO: write this code
        float sum = 0;
        sum += x * other.x;
        sum += y * other.y;
        return sum;
    }
    float Dot(Point2D b) const {
        // TODO: write this code
        float sum = 0;
        sum += x * b.x;
        sum += y * b.y;
        return sum;
    }
    static float Dot(Point2D a, Point2D b) {
        // TODO: write this code
        float sum = 0;
        sum += a.x * b.x;
        sum += a.y * b.y;
        return sum;
    }
    static float Cross(Point2D a, Point2D b) {
        // TODO: write this code
        float sum = 0;
        sum += ((a.x * b.y) - (b.x * a.y));
        return sum;
    }
    void Normalize() {                              //Emit a warning for division by 0
        // TODO: write this code
        float length = sqrt((x * x) + (y * y));
        if (length == 0)
        {
            x = 0;
            y = 0;
        }
        else
        {
            x = x / length;
            y = y / length;
        }
    }
};

static std::ostream &operator<<(std::ostream &os, const Point2D &p) {
    // TODO: write this code
    return os;
}

static Point2D operator*(float number, const Point2D &rhs) {
    // TODO: write this code
    return rhs;
}

struct Line {
    Point2D p1, p2;

    Line(Point2D p1 = {0, 0}, Point2D p2 = {0, 0}) : p1(p1), p2(p2) {}
    Line(float x1, float y1, float x2, float y2) : p1(x1, y1), p2(x2, y2) {}
    float Length() const {
        // The Length between two points p1 and p2
        // Length = sqrt((x2-x1)^2 + (y2-y1)^2)
        float length = 0;
        double posX = (p2.x - p1.x) * (p2.x - p1.x);
        double posY = (p2.y - p1.y) * (p2.y - p1.y);

        length = sqrt(posX + posY);
        //std::cout << "The length is " << length;
        return length;
    }
    Point2D ClosestPoint(const Point2D &p) const {
        // Find closest point on the line from another point p
        // 1) Find x (top) = p1p2 (ab) * p1p3 (ac) / |ab|
        // ab = p2 - p1 = line of p1 to p2
        Point2D ab = p2 - p1;
        
        // ac = p - p1 = line from p1 to p
        Point2D ac = p - p1;

        // bc = p - p2
        Point2D bc = p - p2;

        // If it is out of bounds on the bottom
        if ((ac * ab) < 0.0f) {
            return p1;
        }

        // If it is out of bounds at the top
        if ((ab * bc) > 0.0f) {
            return p2;
        }

        // Get the magnitude of ab
        float magAB = p1.Distance(p2);
        
        // Check for division by zero, if so, then p1 and p2 are the same point
        if (magAB == 0.0f) {
            return p1;
        }

        // Find the projection distance
        float x = (ab * ac) / magAB;
        

        // Calculate the point
        float scalar = x / magAB;
        Point2D answer = p1 + (ab * scalar);
        return answer;
    }
    bool Crosses(Line other, Point2D &crossingPoint) const {
        // Get the direction vectors of the lines
        Point2D vectorA = p2 - p1;
        Point2D vectorB = other.p2 - other.p1;

        // 1) Check for parallel lines
        float crossProduct = Point2D::Cross(vectorA, vectorB);

        if (crossProduct == 0) {
            // The Lines are parallel.
            return false;
        }

        // 2) Find t, u (scalars which is the percentage on the line)
        // u = ((p1 - other.p1) x (vectorA) / (vectorB x vectorA)
        float u = (Point2D::Cross((p1 - other.p1), vectorA)) / -crossProduct;

        // t = ((other.p1 - p1) x vectorB) / (crossProduct)
        float t = (Point2D::Cross((other.p1 - p1), vectorB)) / crossProduct;
        
        // Check if u and t are between [0, 1]
        if (u < 0.0f || u > 1.0f || t < 0.0f || t > 1.0f) {
            return false;
        }

        // Since u and t are in-bounds, there must be an intersection.
        crossingPoint = p1 + (vectorA * t);

        return true;
    }
};

static std::ostream &operator<<(std::ostream &os, const Line &l) {
    // TODO: write this code
    os << "Line from (" << l.p1.x << "," << l.p1.y <<") to (" << l.p2.x << "," << l.p2.y << ")\n";
    return os;
}

struct Circle {
    Point2D center;
    float radius;

    Circle(Point2D c = {0, 0}, float r = 0) : center(c), radius(r) {}

    Circle(float x, float y, float r) : center(x, y), radius(r) {}
};

struct Rect {
    Point2D topLeft;
    float width, height;

    Rect(float left, float top, float width, float height)
        : topLeft(Point2D(top, left)), width(width), height(height) {}

    Rect(Point2D tl = {0, 0}, int w = 0, int h = 0) : topLeft(tl), width(w), height(h) {}

    // Creates bounding box around p1 and p2 with positive width/height
    Rect(Point2D p1, Point2D p2)
        : topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
          width(fabs(p1.x - p2.x)),
          height(fabs(p1.y - p2.y)) {}

    Rect(Point2D center, float radius)
        : topLeft(center.x - radius, center.y - radius), width(2 * radius), height(2 * radius) {}

    Rect &operator|=(const Rect &other) {
        // Bounding Box around both rectangles
        // 1) Find the new Top-left corner
        float topLeft_x = std::min(topLeft.x, other.topLeft.x);
        float topLeft_y = std::min(topLeft.y, other.topLeft.y);

        // Find the bottom Right
        float bottomRight_x = std::max((topLeft.x + width), (other.topLeft.x + other.width));
        float bottomRight_y = std::max((topLeft.y + height), (other.topLeft.y + other.height));
        
        topLeft.x = topLeft_x;
        topLeft.y = topLeft_y;
        width = bottomRight_x - topLeft_x;
        height = bottomRight_y - topLeft_y;
        return *this;
    }
    Rect &operator|=(const Point2D &other) {
        float topLeft_x = std::min(topLeft.x, other.x);
        float topLeft_y = std::min(topLeft.y, other.y);

        float bottomRight_x = std::max((topLeft.x + width), other.x);
        float bottomRight_y = std::max((topLeft.y + height), other.y);

        topLeft.x = topLeft_x;
        topLeft.y = topLeft_y;
        width = bottomRight_x - topLeft_x;
        height = bottomRight_y - topLeft_y;
        return *this;
    }
    Rect &operator|=(const Line &other) {
        // Compare current bounds against BOTH line points
        float newTopLeft_x = std::min({topLeft.x, other.p1.x, other.p2.x});
        float newTopLeft_y = std::min({topLeft.y, other.p1.y, other.p2.y});

        float bottomRight_x = std::max({topLeft.x + width, other.p1.x, other.p2.x});
        float bottomRight_y = std::max({topLeft.y + height, other.p1.y, other.p2.y});

        topLeft.x = newTopLeft_x;
        topLeft.y = newTopLeft_y;
        width = bottomRight_x - newTopLeft_x;
        height = bottomRight_y - newTopLeft_y;

        return *this;
    }
    Rect &operator&=(const Rect &other) {
        // 1) Find the inner-most Top-Left corner
        float newTopLeft_x = std::max(topLeft.x, other.topLeft.x);
        float newTopLeft_y = std::max(topLeft.y, other.topLeft.y);

        // 2) Find the inner-most Bottom-Right corner
        float bottomRight_x = std::min(topLeft.x + width, other.topLeft.x + other.width);
        float bottomRight_y = std::min(topLeft.y + height, other.topLeft.y + other.height);

        // 3) Update variables
        topLeft.x = newTopLeft_x;
        topLeft.y = newTopLeft_y;
        
        // 4) Calculate new dimensions and clamp to 0 if they don't overlap
        width = std::max(0.0f, bottomRight_x - newTopLeft_x);
        height = std::max(0.0f, bottomRight_y - newTopLeft_y);

        return *this;
    }
    Rect &operator+=(const Point2D &other) {
        // Shifts the rectangle by the adding the point's coordinates to the top-left corner
        topLeft += other;
        return *this;
    }
    Rect operator+(const Point2D &other) const {
        // Creates a copy of the currently rectangle, shifts it, and returns the new copy
        Rect result = *this;
        result += other;
        return result;
    }
    void Inset(int inset) {
        // Pushes all four edges inward by the inset amound
        // The top-left corner moves down and right, while the overall size shrinks from both sides.
        topLeft.x += inset;
        topLeft.y += inset;
        width -= (2 * inset);
        height -= (2 * inset);
    }
    bool IsInside(const Point2D &p) const {
        // Checks if the target point's X and Y fall strictly within the rectangle's bounds
        bool withinX = (p.x >= topLeft.x) && (p.x <= topLeft.x + width);
        bool withinY = (p.y >= topLeft.y) && (p.y <= topLeft.y + height);
        return withinX && withinY;
    }
};

static std::ostream &operator<<(std::ostream &os, const Rect &l) {
    os << "Rect(topLeft: (" << l.topLeft.x << ", " << l.topLeft.y 
       << "), width: " << l.width << ", height: " << l.height << ")";
    return os;
}

}  // namespace CMPUT350

#endif  // MATHUTIL_H
