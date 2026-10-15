#include "Player.h"
#include "Bullet.h"

/*
Initialize the Player's Bounding Boxes
Input: loc - the center of the ship in screen pixels.
Output: none. Sets mBounds (body), shipWingBounds, and ShipFrontBounds (nose)
        so the three rectangles together form the ship
*/
Player::Player(CMPUT350::Point2D loc)
{
    mLocation = loc;
    // loc is the center, find top left corner
    
    // Top-left corner of the body
    float left = -kPlayerWidth / 2.0f;
    float top = -kPlayerHeight / 2.0f;
    
    mBounds = CMPUT350::Rect(left, top, kPlayerWidth, kPlayerHeight);
    shipWingBounds = CMPUT350::Rect(left - 15, top + 25, kPlayerWidth + 30, kPlayerHeight - 28);
    ShipFrontBounds = CMPUT350::Rect(left + 14, top - 15, kPlayerWidth - 28, kPlayerHeight + 18);
}

/*
Called once by the engine wheen the Player is added to the game.
Left empty because the constructor already sets up all of the Player's state
*/
void Player::Initialize(CMPUT350::GameContext* context)
{
    for (const std::string message : {"MoveLeftDown", "MoveLeftUp", "MoveRightDown", "MoveRightUp", "Shoot"}) {
        context->mNotificationManager->Register(context->CurrObject, message);
    }
}

void Player::ReceiveNotification(const std::string &message) {
    if (message == "MoveLeftDown") {
        mMovingLeft = true;
    }
    else if (message == "MoveLeftUp") {
        mMovingLeft = false;
    }
    else if (message == "MoveRightDown") {
        mMovingRight = true;
    }
    else if (message == "MoveRightUp") {
        mMovingRight = false;
    }
    else if (message == "Shoot") {
        mFireRequested = true;
    }
}

/*
Called once per frame, after all key events have been handled.
Input: context - gives access to the engine (to add bullets).
Output: none. Counts down the fire cooldown and, if the player asked
        to fire, spawns a bullet when the cooldown is over and one of 
        the two bullet slots is free (its weak_ptr has expired).
*/
void Player::Update(CMPUT350::GameContext* context)
{
    float screenWidth = context->ScreenContext->GetWindowWidth();
    if (mMovingLeft && mLocation.x - kWingHalfWidth - kMoveSpeed >= 0) {
        mLocation.x -= kMoveSpeed;
    }
    if (mMovingRight && mLocation.x + kWingHalfWidth + kMoveSpeed <= screenWidth) {
        mLocation.x += kMoveSpeed;
    }

    if (mCooldown > 0) {
        mCooldown--;
    }

    // Fire here so the bullet uses the ship's position afte this frame's movement
    if (mFireRequested && mCooldown == 0) {
        for (auto& slot : mBullets) {
            
            if (slot.expired()) {
                // Just above the tip of the nose
                CMPUT350::Point2D firePosition(
                    mLocation.x, mLocation.y + ShipFrontBounds.topLeft.y - 5.0f);
                CMPUT350::Point2D direction(0.0f, -1.0f);

                auto bullet = std::make_shared<Bullet>(firePosition, direction, true);
                slot = bullet;
                context->mEngineView->AddGameObject(bullet);

                mCooldown = kFireDelay;
                break;
            }
        }
    }
    mFireRequested = false; 
}

/*
Called once per frame after collisions have been processed
Left empty because the Player has no logic that depends on the
result of collisions, handles in Project 1b.
*/
void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

/*
Handle a key press sent by the engine.
Input: context - gives access to the screen size
       key - the ASCII character that was pressed
Output: true if the Player used the key, false otherwise
        'a' moves left and 'd' moves right (10 pixels, kept on screen)
        ' ' only sets mFireRequested, the bullet is created in Update.
*/
/*
bool Player::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    if (key == 'a')
    {
        //std::cout << context->ScreenContext->GetWindowWidth() << "\n";
        if (mBounds.topLeft.x > 0)
        {
            mBounds.topLeft.x -= 10;
            shipWingBounds.topLeft.x -= 10;
            ShipFrontBounds.topLeft.x -= 10;
            return true;
        }
        else { return false; }
    }
    else if (key == 'd')
    {
        if (mBounds.topLeft.x < (context->ScreenContext->GetWindowWidth() - 40))
        {
            mBounds.topLeft.x += 10;
            shipWingBounds.topLeft.x += 10;
            ShipFrontBounds.topLeft.x += 10;
            return true;
        }
        else { return false; }
    }
    else if (key == ' ')
    {
        mFireRequested = true;
        return true;
    }
    else
    {
        return false;
    }
    
}
*/

/*
Draw anything that must appear behind other objects.
Left empty because the ship is drawn in foreground, so the Player has nothing to
put in the background.
*/
void Player::RenderBackground(CMPUT350::GameContext* context)
{
}

/*
Draw the ship in the foreground layer.
Input: context - gives access to the DrawContext
Output: none. Draws the body, wings, and nose as gray rectangles
*/
void Player::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::gray);
    context->ScreenContext->DrawRect(shipWingBounds, CMPUT350::Colors::gray);
    context->ScreenContext->DrawRect(ShipFrontBounds, CMPUT350::Colors::gray);
}

/*
The center of the ship in screen space. The ship rotates around this point
*/
CMPUT350::Point2D Player::GetLocation() const 
{
    return mLocation;
}
/*
Get the ship's rotation in radians
*/
float Player::GetRotation() const
{
    return mRotation;
}

/*
Called by the engine when the Player's bounding box overlaps another object.
TODO in project 1b
*/
void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
}

/*
Mark the Player as dead
Output: none. The engine removes the Player at the start of the next frame.
*/
void Player::Kill()
{
    mAlive = false;
}

/*
Output: true while the Player is alive. The engine uses this to decide
        when to remove the object
*/
bool Player::IsAlive() const
{
    return mAlive;
}

/*
Output: the Player's main bounding box (the body). The engine uses it
        for collision checks. The wings and nose are not included.
*/
const CMPUT350::Rect& Player::GetBounds()
{
    mWorldBounds = mBounds + mLocation;
    return mWorldBounds;
}

void Player::SetPaused(bool paused)
{
    mIsPaused = paused;
}


