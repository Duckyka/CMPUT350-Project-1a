#include "Bullet.h"
#include "DrawContext.h"
#include "Player.h"
/*
Create a bullet
Input:  location - where the bullet starts, in screen pixels
        heading - the direction it travels
        player - true if the player fired, false if enemy did.

Output: none. Sets the bullet's position, direction, owner, and a small
        square bounding box centered on the location.
*/
Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player)
    : mLocation(location),
      mHeading(heading),
      mIsPlayer(player),
      mAlive(true),
      mBounds(location - (kSize / 2.0f), kSize, kSize)
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
Same as Player, everything was initialized by the constructor
*/
void Bullet::Initialize(CMPUT350::GameContext* context)
{
}

/*
Called once per frame before collisions are processed.
Input:  context - not used here
Output: none. Moves the bullet kSpeed pixels along its heading and
        moves its bounding box to the new position.
*/
void Bullet::Update(CMPUT350::GameContext* context)
{
    // Move along the heading
    mLocation += mHeading * kSpeed;
    mBounds = CMPUT350::Rect(mLocation - (kSize / 2.0f), kSize, kSize);
}

/*
Called once per frame after collisions have been processed.
Input: context - gives access to the window size.
Output: none. Kills the bullet if its center has left the screen, so
        the engine removes it and the player's weak_ptr slot is freed.
*/
void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
    // Check if the bullet leaves the screen, if so then kill
    float screenW = context->ScreenContext->GetWindowWidth();
    float screenH = context->ScreenContext->GetWindowHeight();

    if (mLocation.x < 0 || mLocation.x > screenW ||
        mLocation.y < 0 || mLocation.y > screenH) {
            Kill();
        }
}

/*
Always return false since bullet's dont have keyboard commands.
*/
bool Bullet::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false;
}

void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
}

/*
Draw the bullet in the foreground
*/
void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::RGBColor color = mIsPlayer ? CMPUT350::Colors::cyan
                                         : CMPUT350::Colors::red;
    context->ScreenContext->DrawRect(mBounds, color);
}

/*
Called by the engine when the bullet's bounding box overlaps another object
Input:  obj - the object the bullet hit
Output: none. Bullets ignore other bullets, and player bullets pass
        through the player. Anything else kills the bullet.
*/
void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    // Ignore other bullets
    if (std::dynamic_pointer_cast<Bullet>(obj)) {
        return;
    }

    // Player bullets pass through the player
    if (mIsPlayer && std::dynamic_pointer_cast<Player>(obj)) {
        return;
    }

    Kill();
}

/*
Mark the bullet as dead
*/
void Bullet::Kill()
{
    mAlive = false;
}

/*
return true when the bullet is alive. The engine uses this to 
decide when to remove the object.
*/
bool Bullet::IsAlive() const
{
    return mAlive;
}

/*
The bullet's bounding box. The engine uses it for collision checks.
*/
const CMPUT350::Rect& Bullet::GetBounds()
{
    return mBounds;
}
