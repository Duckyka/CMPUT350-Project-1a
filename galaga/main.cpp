#include <random>
#include "GameEngine.h"
#include "MathUtil.h"
#include "Player.h"
#include "Enemy.h"
#include "Stars.h"
#include "Keyboard.h"
#include "Galaga.h"

// const int numRows = 5;
// const int numCols = 8;
// const float screenWidth = 768.0f;

// namespace Key = CMPUT350::Key;

// class Ball : public CMPUT350::CollisionObject
// {
// public:
//     Ball(int radius) : CollisionObject(), mRadius(radius), mNumCollisions(0), mLoc(radius, radius), mSpeed(3, 3),
//                        mSpeedNext(3, 3), mFrameCollisions(0)
//     {
//         std::uniform_int_distribution<int> xRand(mRadius, 1024 - mRadius);
//         std::uniform_int_distribution<int> yRand(mRadius, 768 - mRadius);
//         std::uniform_int_distribution<int> speed(1, 5);

//         // Initialize ball with a random position and speed
//         mLoc.x = xRand(gen);
//         mLoc.y = yRand(gen);
//         mSpeed.x = speed(gen); // Random speed between 1 and 5
//         mSpeed.y = speed(gen); // Random speed between 1 and 5
//     }

//     void Update(CMPUT350::GameContext* context) override
//     {
//         // Update ball position or state
//         mLoc += mSpeedNext;
//         mSpeed = mSpeedNext;
//         if (mLoc.x > 1024 - mRadius || mLoc.x < mRadius)
//         {
//             mSpeed.x = -mSpeed.x;
//             if (mSpeed.x > 0)
//                 mSpeed.x++;
//         }
//         if (mLoc.y > 768 - mRadius || mLoc.y < mRadius)
//         {
//             mSpeed.y = -mSpeed.y;
//             if (mSpeed.y > 0)
//                 mSpeed.y++;
//         }
//         if (mNumCollisions % 10 == 8)
//         {
//             std::uniform_int_distribution<int> size(50, 150);
//             context->mEngineView->AddGameObject(std::make_shared<Ball>(size(gen)));
//             mNumCollisions++;
//         }
//         mSpeedNext = mSpeed;
//     }

//     void LateUpdate(CMPUT350::GameContext* context) override
//     {
//         if (mFrameCollisions > 0)
//             mNumCollisions++;
//         mFrameCollisions = 0;
//     }

//     void CollisionEnter(const std::shared_ptr<CollisionObject>& obj) override
//     {
//         std::shared_ptr<Ball> ball = std::dynamic_pointer_cast<Ball>(obj);
//         // Only collisions with other Balls
//         if (ball != nullptr)
//         {
//             CMPUT350::Point2D b1 = mLoc;
//             CMPUT350::Point2D b2 = ball->mLoc;
//             if (b1.Distance(b2) <= ball->mRadius + mRadius) // Are the balls within radius
//             {
//                 mFrameCollisions++;
//                 CMPUT350::Point2D normal = b1 - b2;
//                 normal /= normal.Distance(CMPUT350::Point2D(0, 0));
//                 mCollisionPoint = normal * (-mRadius);
//                 float m1 = 3.14f * mRadius * mRadius; // mass of this ball
//                 float m2 = 3.14f * ball->mRadius * ball->mRadius; // mass of other ball
//                 CMPUT350::Point2D v1prime = mSpeed - (2 * m2 / (m1 + m2)) * ((normal * (mSpeed - ball->mSpeed)) *
//                     normal);
//                 mSpeedNext = v1prime;
//             }
//         }
//     }

//     const CMPUT350::Rect& GetBounds() override
//     {
//         static CMPUT350::Rect bounds({0, 0}, 0, 0); // Initialize with dummy values
//         bounds = CMPUT350::Rect(mLoc - mRadius, 2 * mRadius, 2 * mRadius);
//         return bounds;
//     }

//     // One circle centered on the ball, in local space
//     const std::vector<CMPUT350::Shape>& GetShapes() override
//     {
//         mShapes = {CMPUT350::Shape(CMPUT350::Circle(CMPUT350::Point2D(0, 0), mRadius))};
//         return mShapes;
//     }

//     bool IsAlive() const override
//     {
//         return mNumCollisions < 10;
//     }

//     CMPUT350::Point2D GetLocation() const override
//     {
//         return mLoc;
//     }

//     float GetRotation() const override
//     {
//         return 0.0f;
//     }

//     void RenderForeground(CMPUT350::GameContext* context) override
//     {
//     context->ScreenContext->DrawCircle({0, 0}, mRadius, CMPUT350::Colors::red);
//     context->ScreenContext->DrawCenteredText("Circle" + std::to_string(mNumCollisions), mRadius / 6.0f, {0, 0},
//                                              CMPUT350::Colors::white);
//     context->ScreenContext->DrawLine({0, 0}, mCollisionPoint, mRadius * 0.1f, CMPUT350::Colors::blue);
//     }

// private:
//     static std::random_device rd;
//     static std::mt19937 gen;

//     int mRadius, mNumCollisions, mFrameCollisions;
//     CMPUT350::Point2D mLoc, mSpeed, mSpeedNext, mCollisionPoint;
//     std::vector<CMPUT350::Shape> mShapes;
// };

// // Shared randoms across all balls
// // These are the actual objeects associated with the class. Normally these
// // would be in a C++ file
// std::random_device Ball::rd;
// std::mt19937 Ball::gen(rd());

// int main()
// {
//     bool mBallSsample = false;

//     if (mBallSsample)
//     {
//         CMPUT350::GameEngine engine(1024, 768, "Ball Simulation");
//         engine.AddGameObject(std::make_shared<Ball>(100));
//         engine.AddGameObject(std::make_shared<Ball>(50));
//         engine.Run();
//     }
const bool gUseCollisionDemo = true;

class Ball : public CMPUT350::CollisionObject
{
public:
	Ball(CMPUT350::Point2D location, int radius) : CollisionObject(), mRadius(radius), mLoc(location)
	{
		mInCollision = mHasCollisionPoint = false;
		mShapes.push_back(CMPUT350::Shape(CMPUT350::Circle(mLoc.x, mLoc.y, radius)));
		mBounds = CMPUT350::Rect(CMPUT350::Point2D(-mRadius, -mRadius), 2*mRadius, 2*mRadius); // Initialize with dummy values
		
	}
	
	void Update(CMPUT350::GameContext *context) override
	{
		mInCollision = mHasCollisionPoint = false;
	}
	
	void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) override
	{
		mInCollision = true;
		mHasCollisionPoint = false;
	}
	
	void CollisionEnter(const std::shared_ptr<CollisionObject> &obj, const CMPUT350::Point2D &collisionPoint) override
	{
		mInCollision = true;
		mHasCollisionPoint = true;
		mCollisionPoint = collisionPoint;
	}
	
	const CMPUT350::Rect &GetBounds() override
	{
		return mBounds;
	}
	
	//bool IsStatic() const override { return false; }
	
	const std::vector<CMPUT350::Shape> &GetShapes() override
	{
		return mShapes;
	}
	
	bool IsAlive() const override
	{
		return true;
	}
	
	CMPUT350::Point2D GetLocation() const override
	{
		return mLoc;
	}
	
	float GetRotation() const override
	{
		return 0;
	}
	
	void RenderForeground(CMPUT350::GameContext *context) override
	{
		if (mInCollision)
		{
			context->ScreenContext->DrawCircle(mLoc, mRadius, CMPUT350::Colors::red);
		}
		else {
			context->ScreenContext->DrawCircle(mLoc, mRadius, CMPUT350::Colors::blue);
		}
		if (mHasCollisionPoint)
			context->ScreenContext->DrawCircle(mCollisionPoint, 5, CMPUT350::Colors::yellow);
	}
private:
	int mRadius;
	CMPUT350::Point2D mLoc, mCollisionPoint;
	bool mInCollision;
	bool mHasCollisionPoint;
	std::vector<CMPUT350::Shape> mShapes;
	CMPUT350::Rect mBounds;
};

class Line : public CMPUT350::CollisionObject
{
public:
	Line(CMPUT350::Point2D from, CMPUT350::Point2D to) : CollisionObject(), mFrom(from), mTo(to), mRotation(0)
	{
		mInCollision = mHasCollisionPoint = false;
		mShapes.push_back(CMPUT350::Shape(CMPUT350::Line({0, 0}, mTo-mFrom)));
		mBounds = CMPUT350::Rect({0, 0}, mTo-mFrom);
	}
	
	void Update(CMPUT350::GameContext *context) override
	{
		mRotation = (mRotation+3)%360;
		mInCollision = mHasCollisionPoint = false;
	}
	
	void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) override
	{
		mInCollision = true;
		mHasCollisionPoint = false;
	}
	
	void CollisionEnter(const std::shared_ptr<CollisionObject> &obj, const CMPUT350::Point2D &collisionPoint) override
	{
		mInCollision = true;
		mHasCollisionPoint = true;
		mCollisionPoint = collisionPoint;
	}
	
	const CMPUT350::Rect &GetBounds() override
	{
		return mBounds;
	}
	
	//bool IsStatic() const override { return false; }
	
	const std::vector<CMPUT350::Shape> &GetShapes() override
	{
		return mShapes;
	}
	
	bool IsAlive() const override
	{
		return true;
	}
	
	CMPUT350::Point2D GetLocation() const override
	{
		return mFrom;
	}
	
	float GetRotation() const override
	{
		return mRotation*3.14159265358979323846f/180.0f;
	}
	
	void RenderForeground(CMPUT350::GameContext *context) override
	{
		if (mInCollision)
		{
			context->ScreenContext->DrawLine(mTo, mFrom, 2.0f, CMPUT350::Colors::red);
			
		}
		else {
			context->ScreenContext->DrawLine(mTo, mFrom, 2.0f, CMPUT350::Colors::blue);
		}
		if (mHasCollisionPoint)
			context->ScreenContext->DrawCircle(mCollisionPoint, 3, CMPUT350::Colors::yellow);
	}
private:
	int mRadius;
	CMPUT350::Point2D mFrom, mTo, mCollisionPoint;
	bool mInCollision;
	bool mHasCollisionPoint;
	int mRotation;
	std::vector<CMPUT350::Shape> mShapes;
	CMPUT350::Rect mBounds;
};

class Rect : public CMPUT350::CollisionObject
{
public:
	Rect(CMPUT350::Point2D from, CMPUT350::Point2D to) : CollisionObject(), mFrom(from), mTo(to)
	{
		mInCollision = mHasCollisionPoint = false;
		mShapes.push_back(CMPUT350::Shape(CMPUT350::Rect({0, 0}, mTo-mFrom)));
		mBounds = CMPUT350::Rect({0, 0}, mTo-mFrom); // Initialize with dummy values
	}
	
	void Update(CMPUT350::GameContext *context) override
	{
		mRotation -= 0.05f;
		if (mRotation < 0)
			mRotation += 2.0f*3.14159265358979323846f;
		mInCollision = mHasCollisionPoint = false;
	}
	
	void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) override
	{
		mInCollision = true;
		mHasCollisionPoint = false;
	}
	
	void CollisionEnter(const std::shared_ptr<CollisionObject> &obj, const CMPUT350::Point2D &collisionPoint) override
	{
		mInCollision = true;
		mHasCollisionPoint = true;
		mCollisionPoint = collisionPoint;
	}
	
	const CMPUT350::Rect &GetBounds() override
	{
		return mBounds;
	}
	
	//bool IsStatic() const override { return false; }
	
	const std::vector<CMPUT350::Shape> &GetShapes() override
	{
		return mShapes;
	}
	
	bool IsAlive() const override
	{
		return true;
	}
	
	CMPUT350::Point2D GetLocation() const override
	{
		return mFrom;
	}
	
	float GetRotation() const override
	{
		return mRotation;
	}
	
	void RenderForeground(CMPUT350::GameContext *context) override
	{
		if (mInCollision)
		{
			context->ScreenContext->DrawRect(CMPUT350::Rect({0, 0}, mTo-mFrom), CMPUT350::Colors::red);
		}
		else {
			context->ScreenContext->DrawRect(CMPUT350::Rect({0, 0}, mTo-mFrom), CMPUT350::Colors::blue);
		}
		if (mHasCollisionPoint)
			context->ScreenContext->DrawCircle(mCollisionPoint, 3, CMPUT350::Colors::yellow);
	}
private:
	int mRadius;
	CMPUT350::Point2D mFrom, mTo, mCollisionPoint;
	CMPUT350::Rect mBounds;
	bool mInCollision;
	bool mHasCollisionPoint;
	float mRotation;
	std::vector<CMPUT350::Shape> mShapes;
};

int main()
{
	if (gUseCollisionDemo)
	{
		CMPUT350::GameEngine engine(512, 512, "Collision Demo");
		engine.AddGameObject(std::make_shared<Ball>(CMPUT350::Point2D(100, 100), 25));
		engine.AddGameObject(std::make_shared<Ball>(CMPUT350::Point2D(150, 100), 25));

		engine.AddGameObject(std::make_shared<Ball>(CMPUT350::Point2D(300, 100), 25));
		engine.AddGameObject(std::make_shared<Rect>(CMPUT350::Point2D(350, 90), CMPUT350::Point2D(400, 105)));
		engine.AddGameObject(std::make_shared<Ball>(CMPUT350::Point2D(360, 70), 5));

		engine.AddGameObject(std::make_shared<Ball>(CMPUT350::Point2D(400, 400), 25));
		engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(350, 350), CMPUT350::Point2D(390, 350)));
		engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(75, 350), CMPUT350::Point2D(100, 375)));
		engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(100, 350), CMPUT350::Point2D(75, 375)));

		engine.AddGameObject(std::make_shared<Rect>(CMPUT350::Point2D(400-25, 400-125), CMPUT350::Point2D(400+50, 400-100+20)));
		engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(350, 350-100), CMPUT350::Point2D(390, 350-100)));

		engine.AddGameObject(std::make_shared<Rect>(CMPUT350::Point2D(256-25, 256-25), CMPUT350::Point2D(256+25, 256+25)));
		engine.AddGameObject(std::make_shared<Rect>(CMPUT350::Point2D(200, 200), CMPUT350::Point2D(225, 225)));

		engine.AddGameObject(std::make_shared<Ball>(CMPUT350::Point2D(250, 400), 25));
		engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(250, 435), CMPUT350::Point2D(250, 455)));

		engine.AddGameObject(std::make_shared<Ball>(CMPUT350::Point2D(175, 400), 25));
		engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(175, 415), CMPUT350::Point2D(175, 440)));

		engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(30, 30), CMPUT350::Point2D(50, 30)));
		engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(40, 30), CMPUT350::Point2D(60, 30)));

		
		engine.Run();
	}
    else
    {
        CMPUT350::GameEngine engine(768, 1024, "Galaga");
        //auto player = std::make_shared<Player>(CMPUT350::Point2D(768 / 2, 900));
        //engine.AddGameObject(player);
        engine.AddGameObject(std::make_shared<Stars>(250, CMPUT350::Rect(0, 0, 768, 1024)));

        auto galaga = std::make_shared<Galaga>(768, 1024);
        engine.AddGameObject(galaga);

        // Add the 40 enemies
        // for (int row = 0; row < numRows; row++) {
        //     for (int col = 0; col < numCols; col++) {
        //         // Each column gets 96 pixels wide of a slot
        //         float slotWidth = screenWidth / numCols;
        //         float x = slotWidth / 2 + col * slotWidth;

        //         // Rows start near the top and are 60 pixels apart
        //         float y = 80 + row * 60;

        //         auto enemy = std::make_shared<Enemy>(CMPUT350::Point2D(x,y));
        //         engine.AddGameObject(enemy);
        //     }
        // }

        // Temporary key setup for testing
        // engine.InstallKeyDownNotification(Key::A, "MoveLeftDown");
        // engine.InstallKeyDownNotification(Key::Left, "MoveLeftDown");
        // engine.InstallKeyUpNotification(Key::A, "MoveLeftUp");
        // engine.InstallKeyUpNotification(Key::Left, "MoveLeftUp");
        // engine.InstallKeyDownNotification(Key::D, "MoveRightDown");
        // engine.InstallKeyDownNotification(Key::Right, "MoveRightDown");
        // engine.InstallKeyUpNotification(Key::D, "MoveRightUp");
        // engine.InstallKeyUpNotification(Key::Right, "MoveRightUp");
        // engine.InstallKeyDownNotification(Key::Space, "Shoot");

        engine.Run();
    }
    return 0;
}
