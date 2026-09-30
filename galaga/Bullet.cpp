#include "Bullet.h"
#include "DrawContext.h"
#include "Player.h"

Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player)
    : mLocation(location),
      mHeading(heading),
      mIsPlayer(player),
      mAlive(true),
      mBounds(location - (kSize / 2.0f), kSize, kSize)
{
    mHeading.Normalize();
    mBounds = CMPUT350::Rect(mLocation - (kSize / 2.0f), kSize, kSize);
}

bool Bullet::IsPlayerBullet()
{
    // TODO: Update
    return mIsPlayer;
}

void Bullet::Initialize(CMPUT350::GameContext* context)
{
}

void Bullet::Update(CMPUT350::GameContext* context)
{
    // Move along the heading
    mLocation += mHeading * kSpeed;
    mBounds = CMPUT350::Rect(mLocation - (kSize / 2.0f), kSize, kSize);
}

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

bool Bullet::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false;
}

void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
}

void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::RGBColor color = mIsPlayer ? CMPUT350::Colors::cyan
                                         : CMPUT350::Colors::red;
    context->ScreenContext->DrawRect(mBounds, color);
}

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

void Bullet::Kill()
{
    mAlive = false;
}

bool Bullet::IsAlive() const
{
    // TODO: Update code
    return mAlive;
}

const CMPUT350::Rect& Bullet::GetBounds()
{
    // TODO: Update code
    return mBounds;
}
