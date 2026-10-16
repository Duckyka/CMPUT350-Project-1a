#include "Bee.h"

Bee::Bee(CMPUT350::Point2D loc) : Enemy(loc)
{
}

/*
TODO: follow the Bee's attack bezier curve toward the target
Output: true if the Bee started an attack, false otherwise
*/
bool Bee::Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target)
{
    return false;
}

/*
Draw the Bee
*/
void Bee::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::yellow);
}
