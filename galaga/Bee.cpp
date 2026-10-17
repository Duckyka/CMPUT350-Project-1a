#include "Bee.h"
#include "Enemy.h"

Bee::Bee(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& entryPath) : Enemy(loc, entryPath)
{
}

/*
The Bee swoops out to the left, then curves back down toward the target.
Output: true if the Bee started an attack, false otherwise
*/
bool Bee::Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target)
{
    // already busy
    if (mState != EnemyState::InFormation) {
        return false;
    }

    // Start and end of the curve
    CMPUT350::Point2D start = mLocation;
    CMPUT350::Point2D end = KeepOnScreen(context, target);

    // Pull the curve out to the left and down
    CMPUT350::Point2D pull1 = KeepOnScreen(context, start + CMPUT350::Point2D(-250.0f, 150.0f));
    // Pull the curve so the Bee comes down onto the target from above
    CMPUT350::Point2D pull2 = KeepOnScreen(context, end + CMPUT350::Point2D(0.0f, -400.0f));

    FollowPath({start, pull1, pull2, end});
    mState = EnemyState::Attacking;
    return true;
}

/*
Draw the Bee
*/
void Bee::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::yellow);
}
