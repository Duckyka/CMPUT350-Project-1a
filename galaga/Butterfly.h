#ifndef BUTTERFLY_H
#define BUTTERFLY_H

#include "Enemy.h"

class Butterfly : public Enemy
{
public:
    Butterfly(CMPUT350::Point2D loc);

    bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
};

#endif
