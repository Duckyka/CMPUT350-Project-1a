#include "Boss.h"

Boss::Boss(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& entryPath) : Enemy(loc, entryPath)
{
    // Bosses take 2 hits
    mHealth = 2;
}

/*
The Boss rises a little, then waves left and right as it dives at the target
Path is 3 cubic curves
Output: true if the Boss started an attack, false otherwise
*/
bool Boss::Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target)
{
    // already busy
      if (mState != EnemyState::InFormation) {
          return false;
      }

      CMPUT350::Point2D start = mLocation;
      CMPUT350::Point2D end = KeepOnScreen(context, target);

      // How far each third of the dive drops, used for the curve handles
      float drop = (end.y - start.y) / 3.0f * 0.4f;
      // Two weave points, 1/3 and 2/3 of the way down, pushed right then left
      CMPUT350::Point2D weave1 = start + (end - start) * (1.0f / 3.0f) + CMPUT350::Point2D(150.0f, 0.0f);
      CMPUT350::Point2D weave2 = start + (end - start) * (2.0f / 3.0f) + CMPUT350::Point2D(-150.0f, 0.0f);
      weave1 = KeepOnScreen(context, weave1);
      weave2 = KeepOnScreen(context, weave2);

      // Handles straight above and below each weave point, so the ship
      // passes through it heading down and the curves join smoothly
      CMPUT350::Point2D up(0.0f, -drop);
      CMPUT350::Point2D down(0.0f, drop);

      // Curve 1: rise out of formation, then swing right into weave1
      CMPUT350::Point2D p1 = KeepOnScreen(context, start + CMPUT350::Point2D(0.0f, -80.0f));
      CMPUT350::Point2D p2 = KeepOnScreen(context, weave1 + up);
      // Curve 2: weave1 -> weave2 (swing left)
      CMPUT350::Point2D p4 = KeepOnScreen(context, weave1 + down);
      CMPUT350::Point2D p5 = KeepOnScreen(context, weave2 + up);
      // Curve 3: weave2 -> target (come down from above)
      CMPUT350::Point2D p7 = KeepOnScreen(context, weave2 + down);
      CMPUT350::Point2D p8 = KeepOnScreen(context, end + up);

      BeginAttack({start, p1, p2, weave1, p4, p5, weave2, p7, p8, end}, target);
      return true;
}

/*
Draw the Boss
*/
void Boss::RenderForeground(CMPUT350::GameContext* context)
{
    // Green at full health, magenta after being hit once
    CMPUT350::RGBColor color = (mHealth >= 2) ? CMPUT350::Colors::green : CMPUT350::Colors::magenta;
    context->ScreenContext->DrawRect(mBounds, color);
}
