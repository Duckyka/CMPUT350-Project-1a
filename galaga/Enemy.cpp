#include "Enemy.h"
#include "Bullet.h"

namespace {
    constexpr float kEnemyWidth = 30.0f;
    constexpr float kEnemyHeight = 25.0f;
}

/*
Initialize the Enemy's bounding box
*/
Enemy::Enemy(CMPUT350::Point2D loc)
{
    mLocation = loc;
    mBounds = CMPUT350::Rect(-kEnemyWidth / 2.0f, -kEnemyHeight / 2.0f, kEnemyWidth, kEnemyHeight);
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



void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
}

/*
Draw the enemy
*/
void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::red);
}

CMPUT350::Point2D Enemy::GetLocation() const {
    return mLocation;
}

float Enemy::GetRotation() const
{
    return 0.0f;
}

/*
If the bullet collides with the Enemy's bounding box, then delete the Enemy from
the game.
Only react to player bullets
*/
void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    auto bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet && bullet->IsPlayerBullet()) {
        Kill();
    }
}

/*
Mark the Enemy as dead
Output: none. The engine removes the Enemy at the start of the next frame.
*/
void Enemy::Kill()
{
    mAlive = false;
}

/*
Output: true while the Enemy is alive. The engine uses this to decide
        when to remove the object
*/
bool Enemy::IsAlive() const
{
    return mAlive;
}

/*
Output: the Enemy's main bounding box (the body). The engine uses it
        for collision checks.
*/
const CMPUT350::Rect& Enemy::GetBounds()
{
    mWorldBounds = mBounds + mLocation;
    return mWorldBounds;
}

void Enemy::StartAttackRun()
{
    
}