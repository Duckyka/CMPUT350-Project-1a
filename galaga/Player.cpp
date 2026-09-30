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
    shipWingBounds = CMPUT350::Rect({topLeft.x - 15, topLeft.y + 25}, kPlayerWidth + 30, kPlayerHeight - 28);
    ShipFrontBounds = CMPUT350::Rect({topLeft.x + 14, topLeft.y - 15}, kPlayerWidth - 28, kPlayerHeight + 18);
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
    if (key == 'a')
    {
        std::cout << context->ScreenContext->GetWindowWidth() << "\n";
        if (mBounds.topLeft.x > 0)
        {
            mBounds.topLeft.x -= 10;
            shipWingBounds.topLeft.x -= 10;
            ShipFrontBounds.topLeft.x -= 10;
            return true;
        }
        else { return false; }
    }
    else if (key == 'd')
    {
        if (mBounds.topLeft.x < (context->ScreenContext->GetWindowWidth() - 40))
        {
            mBounds.topLeft.x += 10;
            shipWingBounds.topLeft.x += 10;
            ShipFrontBounds.topLeft.x += 10;
            return true;
        }
        else { return false; }
    }
    else if (key == ' ')
    {
        // Spawn at the horizontal center of the ship
        CMPUT350::Point2D firePosition(
            mBounds.topLeft.x + mBounds.width / 2.0f,
            ShipFrontBounds.topLeft.y - 5.0f
        );

        // Screen y grows downward, so "up" is (0, -1)
        CMPUT350::Point2D direction(0.0f, -1.0f);

        context->mEngineView->AddGameObject(
            std::make_shared<Bullet>(firePosition, direction, true));
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
    context->ScreenContext->DrawRect(shipWingBounds, CMPUT350::Colors::gray);
    context->ScreenContext->DrawRect(ShipFrontBounds, CMPUT350::Colors::gray);
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
