#include "Boss.h"

Boss::Boss(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& entryPath) : Enemy(loc, entryPath)
{
}

/*
TODO: follow the Boss's attack bezier curve toward the target
Output: true if the Boss started an attack, false otherwise
*/
bool Boss::Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target)
{
    return false;
}

/*
Draw the Boss
*/
void Boss::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::green);
}
