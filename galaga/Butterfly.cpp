#include "Butterfly.h"
#include "Enemy.h"

Butterfly::Butterfly(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& entryPath) : Enemy(loc, entryPath)
{
}

/*
The Butterfly swings out sideways, does a full loop, then dives at the target
Path is 2 cubic curves
Output: true if the Butterfly started an attack, false otherwise
*/
bool Butterfly::Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target)
{
    // already busy
    if (mState != EnemyState::InFormation) {
        return false;
    }

    // Loop toward the middle of the screen: right if on the left half, 
    // left it on the right half
    float screenMiddle = context->ScreenContext->GetWindowWidth() / 2.0f;
    float side = (mLocation.x < screenMiddle) ? 1.0f : -1.0f;

    CMPUT350::Point2D start = mLocation;
    CMPUT350::Point2D end = KeepOnScreen(context, target);

    // First curve: hop up and out to the side, then swing down into the loop
    CMPUT350::Point2D p1 = KeepOnScreen(context, start + CMPUT350::Point2D(side * 150.0f, -80.0f));
    CMPUT350::Point2D p2 = KeepOnScreen(context, start + CMPUT350::Point2D(side * 250.0f, 100.0f));
    CMPUT350::Point2D p3 = KeepOnScreen(context, start + CMPUT350::Point2D(side * 100.0f, 200.0f));
    // Second curve: mirror p2 so the loop joins smoothly, then dive onto the target
    CMPUT350::Point2D p4 = KeepOnScreen(context, p3 + (p3 - p2));
    CMPUT350::Point2D p5 = KeepOnScreen(context, end + CMPUT350::Point2D(0.0f, -300.0f));

    FollowPath({start, p1, p2, p3, p4, p5, end});
    mState = EnemyState::Attacking;
    return true;
}

/*
Draw the Butterfly
*/
void Butterfly::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::red);
}
