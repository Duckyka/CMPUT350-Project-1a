#include <cassert>
#include "Player.h"
#include "Bullet.h"

namespace {
    constexpr float kPlayerWidth = 40.0f;
    constexpr float kPlayerHeight = 40.0f;
}

Player::Player(CMPUT350::Point2D loc)
{
    // loc is the center, find top left corner
    CMPUT350::Point2D topLeft(
        loc.x - kPlayerWidth / 2.0f,
        loc.y - kPlayerHeight / 2.0f
    );
    mBounds = CMPUT350::Rect(topLeft, kPlayerWidth, kPlayerHeight);
}

void Player::Initialize(CMPUT350::GameContext* context)
{
}

void Player::Update(CMPUT350::GameContext* context)
{
}

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Player::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    std::cout << "In Player HandleKeyEvent()\n";
    if (key == 'a')
    {
        std::cout << "A pressed, Move left\n";
        return true;
    }
    else if (key == 'd')
    {
        std::cout << "D pressed, Move right\n";
        return true;
    }
    else if (key == ' ')
    {
        std::cout << "Space pressed, Shoot a bullet (if possible)\n";
        return true;
    }
    else
    {
        return false;
    }
}

void Player::RenderBackground(CMPUT350::GameContext* context)
{
}

void Player::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::gray);
}

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
}

void Player::Kill()
{
    mAlive = false;
}

bool Player::IsAlive() const
{
    // TODO: Update code
    return mAlive;
}

const CMPUT350::Rect& Player::GetBounds()
{
    // TODO: Update code
    return mBounds;
}
