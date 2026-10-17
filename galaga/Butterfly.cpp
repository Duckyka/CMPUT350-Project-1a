#include "Butterfly.h"

Butterfly::Butterfly(CMPUT350::Point2D loc) : Enemy(loc)
{
}

/*
TODO: follow the Butterfly's attack bezier curve toward the target
Output: true if the Butterfly started an attack, false otherwise
*/
bool Butterfly::Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target)
{
    return false;
}

/*
Draw the Butterfly
*/
void Butterfly::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::red);
}
