#include "DrawContext.h"
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Vector2.hpp>

namespace CMPUT350 {

/**
 * @brief Helper: Convert RGBColor to SFML's sf::Color
 *
 * @param c An RGB type colour
 */
static sf::Color ToSFMLColor(const RGBColor& c) {
    return sf::Color(c.r, c.g, c.b);
}

//Initializer
DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window), mFont(font) {}


/**
 * @brief Takes in text and draws in the center to the screen using the loaded font
 *
 * @param text Text input to write to the screen
 * @param pixelSize Int value for the size of the text
 * @param p Point2D of the text position to draw to
 * @param c RGB colour for the text
 */
void DrawContext::DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    if (!mWindow || !mFont) return;

    sf::Text sfText(*mFont);
    sfText.setString(text);
    sfText.setCharacterSize(pixelSize);
    sfText.setFillColor(ToSFMLColor(c));

    // Measure the text and set the origin to its center
    sf::FloatRect bounds = sfText.getLocalBounds();
    sfText.setOrigin(sf::Vector2f(
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    ));

    // Move and turn the text with the object
    Point2D screenPoint = Transform(p);
    sfText.setPosition(sf::Vector2f(screenPoint.x, screenPoint.y));
    sfText.setRotation(sf::radians(mRotation));

    mWindow->draw(sfText);
}

/**
 * @brief Takes in text and draws to the screen using the loaded font
 *
 * @param text Text input to write to the screen
 * @param pixelSize Int value for the size of the text
 * @param p Point2D of the text position to draw to
 * @param c RGB colour for the text
 */
void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    if (!mWindow || !mFont) return;

    sf::Text sfText(*mFont);
    sfText.setString(text);
    sfText.setCharacterSize(pixelSize);
    sfText.setFillColor(ToSFMLColor(c));

    // Move and turn the text with the object
    Point2D screenPoint = Transform(p);
    sfText.setPosition(sf::Vector2f(screenPoint.x, screenPoint.y));
    sfText.setRotation(sf::radians(mRotation));

    mWindow->draw(sfText);
}

/**
 * @brief Takes in a point and radius and draws to the screen a circle
 *
 * @param p Point2D of the circle position to draw to
 * @param radius Float value for the size of the circle's radius
 * @param c RGB colour for the circle
 */
void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    if (!mWindow) return;

    sf::CircleShape circle(radius);
    circle.setOrigin(sf::Vector2f(radius, radius));
    circle.setFillColor(ToSFMLColor(c));
    // Move the center from the object's local space to the screen
    Point2D center = Transform(p);
    circle.setPosition(sf::Vector2f(center.x, center.y));

    mWindow->draw(circle);
}

/**
 * @brief Takes in a rectangle and draws to the screen
 *
 * @param r Rect to draw to the screen
 * @param c RGB colour for the rectangle
 */
void DrawContext::DrawRect(Rect r, RGBColor c) {
    if (!mWindow) return;

    Point2D corners[4] = {
        r.topLeft,
        {r.topLeft.x + r.width, r.topLeft.y},
        {r.topLeft.x + r.width, r.topLeft.y + r.height},
        {r.topLeft.x, r.topLeft.y + r.height}
    };

    // Move and turn each corner, so a rotated object draws a rotated rectangle
    sf::ConvexShape shape(4);
    for (int i = 0; i < 4; i++) {
        Point2D p = Transform(corners[i]);
        shape.setPoint(i, sf::Vector2f(p.x, p.y));
    }
    shape.setFillColor(ToSFMLColor(c));

    mWindow->draw(shape);
}

/**
 * @brief Takes in a rect to draw to the screen and outlines it in a frame
 *
 * @param r Rect to draw to the screen
 * @param width Float value for the size of the rectangle's outline
 * @param c RGB colour for the rectangle
 */
void DrawContext::FrameRect(Rect r, float width, RGBColor c) {
    if (!mWindow) return;

    Point2D topLeft = r.topLeft;
    Point2D topRight(r.topLeft.x + r.width, r.topLeft.y);
    Point2D bottomRight(r.topLeft.x + r.width, r.topLeft.y + r.height);
    Point2D bottomLeft(r.topLeft.x, r.topLeft.y + r.height);

    // Draw the 4 edges as lines. DrawLine moves and turns them with the object.
    DrawLine(topLeft, topRight, width, c);
    DrawLine(topRight, bottomRight, width, c);
    DrawLine(bottomRight, bottomLeft, width, c);
    DrawLine(bottomLeft, topLeft, width, c);
}

/**
 * @brief Draws a line between two points with a specified width and color.
 *
 * @param from The starting point of the line (Point2D).
 * @param to The ending point of the line (Point2D).
 * @param width The width of the line in pixels.
 * @param c The color of the line, specified as an RGBColor object.
 *
 * This function calculates the distance and angle between the two points
 * and uses a polygon shape to represent the line. The points are in the
 * object's local space and are moved and turned to the screen first.
 */
void DrawContext::DrawLine(Point2D from, Point2D to, float width, RGBColor c) {
    if (!mWindow) return;

    // Move both ends from the object's local space to the screen
    from = Transform(from);
    to = Transform(to);

    float deltaX = to.x - from.x;
    float deltaY = to.y - from.y;

    float length = std::sqrt(deltaX * deltaX + deltaY * deltaY);
    if (length == 0.0f) return;
    
    // Perpendicular direction vector (Δy, -Δx), normalized and scaled to half-width
    Point2D end = {
        (deltaY / length) * (width / 2.f),
        (-deltaX / length) * (width / 2.f)
    };

    Point2D p1Plus  = { from.x + end.x, from.y + end.y };
    Point2D p1Minus = { from.x - end.x, from.y - end.y };
    Point2D p2Plus  = { to.x + end.x,   to.y + end.y };
    Point2D p2Minus = { to.x - end.x,   to.y - end.y };

    // Now to draw the line
    sf::ConvexShape line;
    line.setPointCount(4);
    line.setPoint(0, sf::Vector2f(p1Plus.x, p1Plus.y));
    line.setPoint(1, sf::Vector2f(p2Plus.x, p2Plus.y));
    line.setPoint(2, sf::Vector2f(p2Minus.x, p2Minus.y));
    line.setPoint(3, sf::Vector2f(p1Minus.x, p1Minus.y));
    line.setFillColor(ToSFMLColor(c));

    mWindow->draw(line);
}

//Returns the width of the game area (the view), not the scaled-down window
int DrawContext::GetWindowWidth() { return static_cast<int>(mWindow->getView().getSize().x); }

//Returns the height of the game area (the view), not the scaled-down window
int DrawContext::GetWindowHeight() { return static_cast<int>(mWindow->getView().getSize().y); }

// Remembers where the current object is on the screen
void DrawContext::SetContextOffset(Point2D p)
{
    mOffset = p;
}

// Remembers the current object's rotation in radians
void DrawContext::SetContextRotation(float rotation)
{
    mRotation = rotation;
}

/*
Converts a point from the object's own space to screen space

The object draws as if its center is at (0,0). To find where that point goes
on the screen, we do:
1. Spin the point around (0,0) by the object's rotation
2. Slide the point over to where the object actually is
*/
Point2D DrawContext::Transform(Point2D p)
{
    // cos and sin of the angle tell us how much of the old x and old y
    // end up in the new x and new y after turning
    float c = std::cos(mRotation);
    float s = std::sin(mRotation);

    // Rotation formula: turning (x, y) by an angle gives
    // new x = x*cos - y*sin
    // new y = x*sin + y*cos
    return Point2D(p.x * c - p.y * s + mOffset.x,
                    p.x * s + p.y * c + mOffset.y);
}

/*
Converts a point from screen space back to the object's own space

Undos Transform.
*/
Point2D DrawContext::ReverseTransform(Point2D p)
{
    // 1. Undo the slide
    Point2D local = p - mOffset;

    // 2. Undo the spin by turning the same amount in the opposite direction
    float c = std::cos(-mRotation);
    float s = std::sin(-mRotation);
    return Point2D(local.x * c - local.y * s,
                    local.x * s + local.y * c);
}

/*
Converts a rectangle from the object's own space to screen space

When a rectangle is turned, it's no longer lined up with the screen edges,
so this returns the smallest screen-aligned box that fits around it.
*/
Rect DrawContext::Transform(Rect r)
{
    // Find the top-left and bottom-right corners of the original rectangle
    Point2D tl = r.topLeft;
    Point2D br(r.topLeft.x + r.width, r.topLeft.y + r.height);

    // Turn and move two opposite corners, then make a box around them
    Rect result(Transform(tl), Transform(br));

    // Turn and move the other two corners, and grow the box to fit them too.
    // After a rotation, any of the four corners could end up sticking out
    // the furthest, so we need all four
    result |= Transform(Point2D(br.x, tl.y)); // top-right corner
    result |= Transform(Point2D(tl.x, br.y)); // bottom-left corner
    return result;
}

}  // namespace CMPUT350
