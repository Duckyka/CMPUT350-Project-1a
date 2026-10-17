#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"

namespace {
    constexpr int kSamplesPerSegment = 100;
    constexpr float kPathSpeed = 6.0f;

    /*
    Sample a chain of cubic bezier segments at t in [0, 1] (from Lab 5)
    pts must hold 3n + 1 points: segment k uses pts[3k] .. pts[3k + 3]
    */
    CMPUT350::Point2D GetBezierPoint(const std::vector<CMPUT350::Point2D>& pts, float t)
    {
        // Find which segment t falls in, and resize t to [0,1]
        int numSegments = (static_cast<int>(pts.size()) - 1) / 3;
        float scaled = t * numSegments;
        int seg = static_cast<int>(scaled);
        if (seg >= numSegments) {
            // when t == 1
            seg = numSegments - 1;
        }
        float localT = scaled - seg;
        int base = seg * 3;

        // B(t) = (1−t)^3xP0 + 3(1−t)^2txP1 + 3(1−t)t^2xP2 + t^3xP3
        float u = 1.0f - localT;

        float b0 = u * u * u;
        float b1 = 3.0f * u * u * localT;
        float b2 = 3.0f * u * localT * localT;
        float b3 = localT * localT * localT;

        return CMPUT350::Point2D(
            b0 * pts[base].x + b1 * pts[base + 1].x + b2 * pts[base + 2].x + b3 * pts[base + 3].x,
            b0 * pts[base].y + b1 * pts[base + 1].y + b2 * pts[base + 2].y + b3 * pts[base + 3].y
        );
    }
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
        case EnemyState::Entering:
        case EnemyState::Attacking:
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
        context->mNotificationManager->Notify("EnemyDied");
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
If a player bullet or the player itself touches the Enemy's bounding box,
the Enemy is destroyed.
Everything else (other enemies, enemy bullets) is ignored.
*/
void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    auto bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet && bullet->IsPlayerBullet()) {
        Kill();
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
Output: the Enemy's main bounding box (the body). The engine uses it
        for collision checks.

When the enemy turns, its drawn rectangle turns too, but the engine can only
check boxes that line up with the screen. So we build the smallest
screen-aligned box that still fits around the turned rectangle.
*/
const CMPUT350::Rect& Enemy::GetBounds()
{
    float c = std::fabs(std::cos(mRotation));
    float s = std::fabs(std::sin(mRotation));

    // Width and height of the box around the turned rectangle
    float width = mBounds.width * c + mBounds.height * s;
    float height = mBounds.width * s + mBounds.height * c;

    // mBounds is centered on the enemy, so center the new box on the enemy too
    mWorldBounds = CMPUT350::Rect(mLocation.x - width / 2.0f, mLocation.y - height / 2.0f, width, height);
    return mWorldBounds;
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

void Enemy::StartAttackRun()
{

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

    int numSegments = (static_cast<int>(controlPoints.size()) - 1) / 3;
    int samples = kSamplesPerSegment * numSegments;

    mPath.clear();
    for (int i = 0; i <= samples; i++) {
        float t = static_cast<float>(i) / samples;
        mPath.push_back(GetBezierPoint(controlPoints, t));
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
