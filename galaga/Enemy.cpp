#include "Enemy.h"
#include "Bullet.h"

namespace {
    constexpr float kEnemyWidth = 40.0f;
    constexpr float kEnemyHeight = 40.0f;
}

Enemy::Enemy(CMPUT350::Point2D loc)
{
    // loc is the center, find top left corner
    CMPUT350::Point2D topLeft(
        loc.x - kEnemyWidth / 2.0f,
        loc.y - kEnemyHeight / 2.0f
    );
    mBounds = CMPUT350::Rect(topLeft, kEnemyWidth, kEnemyHeight);
}

void Enemy::Initialize(CMPUT350::GameContext* context)
{
}

void Enemy::Update(CMPUT350::GameContext* context)
{
}

void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Enemy::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false;
}

void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
}

void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::red);
}

void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    if (std::dynamic_pointer_cast<Bullet>(obj)) {
        Kill();
    }
}

void Enemy::Kill()
{
    mAlive = false;
}

bool Enemy::IsAlive() const
{
    // TODO: Update code
    return mAlive;
}

const CMPUT350::Rect& Enemy::GetBounds()
{
    // TODO: Update code
    return mBounds;
}
