#ifndef ENEMY_H
#define ENEMY_H

#include "CollisionObject.h"
#include "GameContext.h"
#include <vector>
#include <string>

enum class EnemyState
{
    Entering,
    InFormation,
    Attacking,
    Returning
};

class Enemy : public CMPUT350::CollisionObject
{
public:
    Enemy(CMPUT350::Point2D formationLoc, const std::vector<CMPUT350::Point2D>& entryPath);
    bool IsInFormation() const;

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
    const CMPUT350::Rect& GetBounds() override;
    const std::vector<CMPUT350::Shape>& GetShapes() override;

    virtual bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) = 0;
    virtual std::string GetTypeName() const = 0;

protected:
    // Body rectangle in local space, centered on (0, 0)
    CMPUT350::Rect mBounds;
    // Collision shapes in local space, returned by GetShapes()
    std::vector<CMPUT350::Shape> mShapes;
    CMPUT350::Point2D mLocation;
    bool mAlive = true;
    static constexpr float kEnemyWidth = 40.0f;
    static constexpr float kEnemyHeight = 30.0f;
    static constexpr float kPathSpeed = 12.0f;

    void FollowPath(const std::vector<CMPUT350::Point2D>& controlPoints);
    std::vector<CMPUT350::Point2D> mPath;
    float mPathDistance = 0.0f;
    float mRotation = 0.0f;
    CMPUT350::Point2D mFormationLocation;

    EnemyState mState = EnemyState::Entering;
    // True once Galaga has been told this enemy died, so it is only reported once
    bool mDeathReported = false;

    CMPUT350::Point2D KeepOnScreen(CMPUT350::GameContext* context, CMPUT350::Point2D p) const;
    int mHealth = 1;

    // Attack and hiring
    void BeginAttack(const std::vector<CMPUT350::Point2D>& controlPoints, CMPUT350::Point2D target);
    void TryFire(CMPUT350::GameContext* context);
    CMPUT350::Point2D mAttackTarget;
    int mShotsFired = 0;
    float mPathLength = 0.0f;
    static constexpr int kShotsPerAttack = 2;
    
private:
    bool AdvanceAlongPath();
    void OnPathFinished();
};


#endif

