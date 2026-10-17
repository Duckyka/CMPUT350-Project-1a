#ifndef BULLET_H
#define BULLET_H

#include "CollisionObject.h"
#include "GameContext.h"

class Bullet : public CMPUT350::CollisionObject
{
public:
    Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player,
           std::weak_ptr<CMPUT350::GameObject> parent);
    bool IsPlayerBullet();

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;

    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj,
                        const CMPUT350::Point2D& collisionPoint) override;
    const CMPUT350::Rect& GetBounds() override;
    const std::vector<CMPUT350::Shape>& GetShapes() override;

private:
    // True if this bullet should be stopped by obj
    bool CanHit(const std::shared_ptr<CMPUT350::CollisionObject>& obj);

    CMPUT350::Point2D mLocation;
    // Where the bullet was last frame, used for the collision line
    CMPUT350::Point2D mPrevLocation;
    CMPUT350::Point2D mHeading;
    bool mIsPlayer;
    bool mAlive;
    // The object that fired this bullet
    std::weak_ptr<CMPUT350::GameObject> mParent;

    // Set when the bullet hits something; the hit point is in local space
    bool mHit = false;
    CMPUT350::Point2D mHitPoint;

    CMPUT350::Rect mBounds;
    // Collision shapes in local space, returned by GetShapes()
    std::vector<CMPUT350::Shape> mShapes;

    static constexpr float kSpeed = 20.0f; // Adjustable
    static constexpr int kSize = 6;
    static constexpr float kHitRadius = 4.0f;
};
#endif // BULLET_H
