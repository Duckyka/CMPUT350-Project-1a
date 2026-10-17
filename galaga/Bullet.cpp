#include "Bullet.h"
#include "DrawContext.h"
#include "Player.h"
#include "Enemy.h"

/*
Create a bullet
Input:  location - where the bullet starts, in screen pixels
        heading - the direction it travels
        player - true if the player fired, false if an enemy did
        parent - the object that fired it; the bullet dies when the parent dies
*/
Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player,
               std::weak_ptr<CMPUT350::GameObject> parent)
    : mLocation(location),
      mPrevLocation(location),
      mHeading(heading),
      mIsPlayer(player),
      mAlive(true),
      mParent(parent),
      mBounds(-kSize / 2.0f, -kSize / 2.0f, kSize, kSize)
{
    mHeading.Normalize();
}

/*
Output: true if the player fired this bullet, false if an enemy did.
        Enemies use this to decide whether a bullet can hurt them.
*/
bool Bullet::IsPlayerBullet()
{
    return mIsPlayer;
}

/*
Everything is set up in the constructor
*/
void Bullet::Initialize(CMPUT350::GameContext* context)
{
}

/*
Called once per frame before collisions are processed.
Moves the bullet kSpeed pixels along its heading.
*/
void Bullet::Update(CMPUT350::GameContext* context)
{
    // The object that fired this bullet is gone, so the bullet goes too
    auto parent = mParent.lock();
    if (!parent || !parent->IsAlive()) {
        Kill();
        return;
    }

    // Move along the heading
    mPrevLocation = mLocation;
    mLocation += mHeading * kSpeed;
}

/*
Called once per frame after collisions have been processed.
Kills the bullet if it has left the screen.
*/
void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
    float screenW = context->ScreenContext->GetWindowWidth();
    float screenH = context->ScreenContext->GetWindowHeight();

    if (mLocation.x < 0 || mLocation.x > screenW ||
        mLocation.y < 0 || mLocation.y > screenH) {
        Kill();
    }
}

void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
}

/*
Draw the bullet. Player bullets are cyan, enemy bullets are red.
After a hit, draw a single circle at the point of collision instead.
*/
void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::RGBColor color = mIsPlayer ? CMPUT350::Colors::cyan
                                         : CMPUT350::Colors::red;
    if (mHit) {
        context->ScreenContext->DrawCircle(mHitPoint, kHitRadius, color);
        return;
    }
    context->ScreenContext->DrawRect(CMPUT350::Rect(-kSize / 2.0f, -kSize / 2.0f, kSize, kSize), color);
}

/*
Output: the center of the bullet in screen space.
*/
CMPUT350::Point2D Bullet::GetLocation() const
{
    return mLocation;
}

/*
Output: the bullet's rotation in radians. Bullets don't rotate, so always 0.
*/
float Bullet::GetRotation() const
{
    return 0.0f;
}

/*
Output: true if this bullet should stop when it touches obj.
        Bullets ignore other bullets, player bullets ignore the player,
        and enemy bullets ignore enemies.
*/
bool Bullet::CanHit(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    if (std::dynamic_pointer_cast<Bullet>(obj)) {
        return false;
    }
    if (mIsPlayer && std::dynamic_pointer_cast<Player>(obj)) {
        return false;
    }
    if (!mIsPlayer && std::dynamic_pointer_cast<Enemy>(obj)) {
        return false;
    }
    return true;
}

/*
Called by the engine when there is no single collision point.
Uses the bullet's own location (0, 0 in local space) as the hit point.
*/
void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    CollisionEnter(obj, CMPUT350::Point2D(0.0f, 0.0f));
}

/*
Called by the engine with the first point along the bullet's line that
touched obj, in the bullet's local space.
If the bullet hits more than one thing, keep the point with the largest y.
*/
void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj,
                            const CMPUT350::Point2D& collisionPoint)
{
    if (!CanHit(obj)) {
        return;
    }

    if (!mHit || collisionPoint.y > mHitPoint.y) {
        mHitPoint = collisionPoint;
    }
    mHit = true;
    Kill();
}

/*
Mark the bullet as dead. The engine removes it at the start of the next frame.
*/
void Bullet::Kill()
{
    mAlive = false;
}

/*
Output: true while the bullet is alive. The engine uses this to
        decide when to remove the object.
*/
bool Bullet::IsAlive() const
{
    return mAlive;
}

/*
Output: the bullet's bounding box in local space. It covers the whole line
        from last frame's position to the current one (0, 0).
*/
const CMPUT350::Rect& Bullet::GetBounds()
{
    float half = kSize / 2.0f;
    mBounds = CMPUT350::Rect(-half, -half, kSize, kSize);
    mBounds |= CMPUT350::Rect(mPrevLocation - mLocation - half, kSize, kSize);
    return mBounds;
}

/*
Output: the bullet's collision shape in local space: a line from last
        frame's position to the current one (0, 0). The line points in the
        direction of travel, so the engine finds the first point it hits.
*/
const std::vector<CMPUT350::Shape>& Bullet::GetShapes()
{
    CMPUT350::Line path(mPrevLocation - mLocation, CMPUT350::Point2D(0.0f, 0.0f));
    mShapes = {CMPUT350::Shape(path)};
    return mShapes;
}
