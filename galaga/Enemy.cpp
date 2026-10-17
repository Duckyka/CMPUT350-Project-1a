#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"
#include "Bezier.h"

namespace {
    // How many points to sample from each bezier segment
    constexpr int kSamplesPerSegment = 100;
}

/*
Initialize the Enemy's bounding box and start it flying in along its entry path
*/
Enemy::Enemy(CMPUT350::Point2D formationLoc, const std::vector<CMPUT350::Point2D>& entryPath) {
    mFormationLocation = formationLoc;
    mBounds = CMPUT350::Rect(-kEnemyWidth / 2.0f, -kEnemyHeight / 2.0f, kEnemyWidth, kEnemyHeight);

    // No path give: just sit in the slot
    if (entryPath.empty()) {
        mLocation = formationLoc;
        mState = EnemyState::InFormation;
        return;
    }

    // Start at the beginning of the path (off-screen), not at the slot
    mLocation = entryPath.front();
    FollowPath(entryPath);
    mState = EnemyState::Entering;
}

void Enemy::Initialize(CMPUT350::GameContext* context)
{
}

/*
Move the enemy each frame based on its current state.
In formation it stays in its slot; otherwise it moves along its current path
*/
void Enemy::Update(CMPUT350::GameContext* context)
{
    switch (mState) {
        case EnemyState::InFormation:
            // Sway left and right around the slot
            mLocation = mFormationLocation + CMPUT350::Point2D(std::sin(context->currentFrame * 0.03f) * 20.0f, 0.0f);
            break;
        case EnemyState::Attacking:
            TryFire(context);
            if (AdvanceAlongPath()) {
                OnPathFinished();
            }
            break;
        case EnemyState::Entering:
        case EnemyState::Returning:
            if (AdvanceAlongPath()) {
                OnPathFinished();
            }
            break;
    }
}

/*
Move kPathSpeed pixels further along mPath and update mLocation and mRotation
*/
bool Enemy::AdvanceAlongPath() {
    // Not on a path, so the enemy is resting in formation
    if (mPath.empty()) {
        return true;
    }

    // Move a fixed number of pixels further along the path this frame
    mPathDistance += kPathSpeed;

    // Walk the sampled segments until we find the one that contains mPathDistance
    float travelled = 0.0f;
    for (size_t i = 0; i + 1 < mPath.size(); i++) {
        float segmentLength = mPath[i].Distance(mPath[i + 1]);
        if (travelled + segmentLength >= mPathDistance && segmentLength > 0.0f) {
            // How far into this segment [0, 1]
            float fraction = (mPathDistance - travelled) / segmentLength;
            mLocation = mPath[i] + (mPath[i + 1] - mPath[i]) * fraction;

            // Face the direction the enemy is moving
            CMPUT350::Point2D direction = mPath[i + 1] - mPath[i];
            mRotation = std::atan2(direction.y, direction.x);
            return false;
        }
        travelled += segmentLength;
    }

    // Past the end of the path: snap to the last point and decide what to do next
    mLocation = mPath.back();
    return true;
}

/*
Called when the enemy reaches the end of a path
After an attack, start flying back to the formation spot.
After flying back, stop and sit still in formation, facing forward.
*/
void Enemy::OnPathFinished()
{
    switch (mState) {
        case EnemyState::Entering:
        case EnemyState::Returning:
            mLocation = mFormationLocation;
            mRotation = 0.0f;
            mState = EnemyState::InFormation;
            break;
        
        case EnemyState::Attacking:
            FollowPath({mLocation, mLocation, mFormationLocation, mFormationLocation});
            mState = EnemyState::Returning;
            break;
        
        case EnemyState::InFormation:
            // Not on a path, nothing to do
            break;
    }
}

/*
Tell the Galaga class when this enemy dies.
CollisionEnter has no GameContext, so it only marks the enemy dead and the
notification is sent here, later in the same frame.
*/
void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
    if (!mAlive && !mDeathReported) {
        // Only report once, even if the enemy was hit by two things in one frame
        mDeathReported = true;
        std::string where = IsInFormation() ? "DiedFormation" : "DiedDiving";
        context->mNotificationManager->Notify(GetTypeName() + where);
    }
}



void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
}

/*
Draw the enemy
*/
void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::red);
}

CMPUT350::Point2D Enemy::GetLocation() const {
    return mLocation;
}

float Enemy::GetRotation() const
{
    return mRotation;
}

/*
A player bullet takes away 1 health; the Enemy is destroyed at 0 health.
Touching the player destroys the Enemy right away.
Everything else (other enemies, enemy bullets) is ignored.
*/
void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    // Already dead this frame, nothing more to do
    if (!mAlive) {
        return;
    }

    // A player bullet takes away 1 health
    auto bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet && bullet->IsPlayerBullet()) {
        mHealth--;
        if (mHealth <= 0) {
            Kill();
        }
        return;
    }

    // Flying into the player also destroys the enemy
    if (std::dynamic_pointer_cast<Player>(obj)) {
        Kill();
    }
}

/*
Mark the Enemy as dead
Output: none. The engine removes the Enemy at the start of the next frame.
*/
void Enemy::Kill()
{
    mAlive = false;
}

/*
Output: true while the Enemy is alive. The engine uses this to decide
        when to remove the object
*/
bool Enemy::IsAlive() const
{
    return mAlive;
}

/*
Output: the Enemy's bounding box in local space (its body, centered on (0, 0)).
        The engine moves and turns it to the screen.
*/
const CMPUT350::Rect& Enemy::GetBounds()
{
    return mBounds;
}

/*
Move a control point so the enemy stays fully on screen.
A bezier curve never leaves the area surrounded by its control points, so if
every control point is on screen, the whole path is on screen too.
*/
CMPUT350::Point2D Enemy::KeepOnScreen(CMPUT350::GameContext* context, CMPUT350::Point2D p) const
{
    // Half the diagonal of the enemy: far enough from the edge at any rotation
    float margin = std::sqrt(mBounds.width * mBounds.width + mBounds.height * mBounds.height) / 2.0f;
    float maxX = context->ScreenContext->GetWindowWidth() - margin;
    float maxY = context->ScreenContext->GetWindowHeight() - margin;

    return CMPUT350::Point2D(std::clamp(p.x, margin, maxX), std::clamp(p.y, margin, maxY));
}

/*
Start flying along an attack path toward target.
Subclasses build the control points, then call this from Attack().
*/
void Enemy::BeginAttack(const std::vector<CMPUT350::Point2D>& controlPoints, CMPUT350::Point2D target)
{
    FollowPath(controlPoints);
    mAttackTarget = target;
    mShotsFired = 0;
    mState = EnemyState::Attacking;
}

/*
Fire at the target when the enemy is 20% and 40% of the way along its attack path.
*/
void Enemy::TryFire(CMPUT350::GameContext* context)
{
    // Already fired every shot for this attack
    if (mShotsFired >= kShotsPerAttack || mPathLength <= 0.0f) {
        return;
    }

    // Not far enough along the path for the next shot yet
    float fireAt = mPathLength * 0.2f * (mShotsFired + 1);
    if (mPathDistance < fireAt) {
        return;
    }

    // Fire from just below the enemy, toward the target
    CMPUT350::Point2D firePosition = mLocation + CMPUT350::Point2D(0.0f, kEnemyHeight);
    CMPUT350::Point2D heading = mAttackTarget - firePosition;
    if (heading.y <= 0.0f) {
        // Target is above, so shoot straight down instead
        heading = CMPUT350::Point2D(0.0f, 1.0f);
    }

    // This enemy is the parent, so its bullets die with it
    context->mEngineView->AddGameObject(std::make_shared<Bullet>(firePosition, heading, false, context->CurrObject));
    mShotsFired++;
}

/*
Sample the bezier curve defined by controlPoints into mPath and start following it
controlPoints must hold 3n + 1 points (4, 7, 10, ...), the same layout as Lab 5
*/
void Enemy::FollowPath(const std::vector<CMPUT350::Point2D>& controlPoints)
{
    if (controlPoints.size() < 4 || (controlPoints.size() - 1) % 3 != 0) {
        std::cerr << "FollowPath: need 3n + 1 control points, got " << controlPoints.size() << "\n";
        return;
    }

    Bezier curve(controlPoints);
    int numSegments = Bezier::GetNumSegments(controlPoints);
    int samples = kSamplesPerSegment * numSegments;

    // Sample the curve; t goes from 0 to numSegments
    mPath.clear();
    for (int i = 0; i <= samples; i++) {
        float t = static_cast<float>(i) / kSamplesPerSegment;
        mPath.push_back(curve.GetPoint(t));
    }

    // Total Length of the path, used to decide when to fire
    mPathLength = 0.0f;
    for (size_t i = 0; i + 1 < mPath.size(); ++i) {
        mPathLength += mPath[i].Distance(mPath[i + 1]);
    }

    mPathDistance = 0.0f;
}

/*
Output: the enemy's collision shape in local space (its body rectangle).
        The engine rotates and moves it to screen space before testing.
*/
const std::vector<CMPUT350::Shape>& Enemy::GetShapes()
{
    mShapes = {CMPUT350::Shape(mBounds)};
    return mShapes;
}

/*
Output: true if the enemy is sitting in its formation slot (not entering, attacking or returning)
*/
bool Enemy::IsInFormation() const {
    return mState == EnemyState::InFormation;
}
