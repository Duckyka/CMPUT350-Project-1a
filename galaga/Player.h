#ifndef PLAYER_H
#define PLAYER_H

#include "CollisionObject.h"
#include <array>
#include <memory>
#include <string>

class Bullet;

class Player : public CMPUT350::CollisionObject, public std::enable_shared_from_this<Player>
{
public:
    Player(CMPUT350::Point2D loc);

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string &message) override;
    bool IsAlive() const override;
    void Kill() override;
    // bool HandleKeyEvent(CMPUT350::GameContext* context, char key) override;
    //void ReceiveNotification(const std::string& key) override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;


    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;
    const std::vector<CMPUT350::Shape>& GetShapes() override;

    void SetPaused(bool paused);

private:
    // Ship shape in local space, centered on (0, 0)
    CMPUT350::Rect mBounds;
    CMPUT350::Rect shipWingBounds;
    CMPUT350::Rect ShipFrontBounds;

    // Body rect shifted to screen space, returned by GetBounds()
    CMPUT350::Rect mWorldBounds;
    // Collision shapes in local space, returned by GetShapes()
    std::vector<CMPUT350::Shape> mShapes;
    CMPUT350::Point2D mLocation;
    float mRotation = 0;

    bool mAlive = true;
    bool mDeathReported = false;
    bool mFireRequested = false;
    int mCooldown = 0;
    bool mMovingLeft = false;
    bool mMovingRight = false;
    bool mIsPaused = false;


    static constexpr int kFireDelay = 8;
    static constexpr int kMaxBullets = 2;
    static constexpr float kPlayerWidth = 40.0f;
    static constexpr float kPlayerHeight = 40.0f;
    static constexpr float kWingHalfWidth = kPlayerWidth / 2.0f + 15.0f;
    static constexpr float kMoveSpeed = 10.0f;

    std::array<std::weak_ptr<Bullet>, kMaxBullets> mBullets;

};

#endif
