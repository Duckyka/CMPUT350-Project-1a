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

/*
Draw the enemy
*/
void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::red);
}
/*
If the bullet collides with the Enemy's bounding box, then delete the Enemy from
the game.
*/
void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    if (std::dynamic_pointer_cast<Bullet>(obj)) {
        Kill();
    }
}

/*
Mark the Enemy as dead
Output: none. The engine removes the Player at the start of the next frame.
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
    return mBounds;
}
