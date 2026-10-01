#include "DrawContext.h"
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

    sfText.setPosition(sf::Vector2f(p.x,p.y));

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
    sfText.setPosition(sf::Vector2f(p.x,p.y));

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

    circle.setFillColor(ToSFMLColor(c));
    circle.setPosition(sf::Vector2f(p.x, p.y));

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

    sf::RectangleShape rectangle({r.width, r.height});
    rectangle.setPosition(sf::Vector2f(r.topLeft.x, r.topLeft.y));
    rectangle.setFillColor(ToSFMLColor(c));

    mWindow->draw(rectangle);
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

    sf::RectangleShape rectangle({r.width, r.height});
    rectangle.setPosition(sf::Vector2f(r.topLeft.x, r.topLeft.y));
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setOutlineColor(ToSFMLColor(c));
    rectangle.setOutlineThickness(width);

    mWindow->draw(rectangle);

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
 * and uses a polygone shape to represent the line. The line is drawn
 * relative to the world offset and rendered onto the associated window.
 */
void DrawContext::DrawLine(Point2D from, Point2D to, float width, RGBColor c) {
    if (!mWindow) return;

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

//Returns the window width
int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

//Returns the window height
int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }

}  // namespace CMPUT350
