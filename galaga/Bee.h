#ifndef BEE_H
#define BEE_H

#include "Enemy.h"

class Bee : public Enemy
{
public:
    Bee(CMPUT350::Point2D loc);

    bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
};

#endif
