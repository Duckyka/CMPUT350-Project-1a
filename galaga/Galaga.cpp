#include "Galaga.h"
#include "Bee.h"
#include "Butterfly.h"
#include "Boss.h"
#include <Keyboard.h>
#include <GameObject.h>
#include <DrawContext.h>
#include <memory>
#include <string>
#include "Bezier.h"
#include "MathUtil.h"
#include <cstdlib>

    // struct Line
    // {
    //     float x;
    //     float y;
    // };

    // class GameEngine

Galaga::Galaga(size_t screenWidth, size_t screenHeight)
    : mScreenWidth(screenWidth),
    mScreenHeight(screenHeight),
    mIsAlive(true),
    mCurrentState(GameState::WaitingForCoins),
    mStateTimer(0.0f),
    mCredits(0),
    mScore(0),
    mLives(3),
    mCurrentLevel(1),
    mCurrentWaveIndex(0),
    mShipsSpawnedInWave(0),
    mSpawnTimer(0.0f),
    mActiveEnemyCount(0),
    mAttackCooldown(3.0f) {
        std::cout << "Initialization of the Galaga Object: TEST TEST\n";
};

Galaga::~Galaga() {}

bool Galaga::IsAlive() const
{
    return mIsAlive;
}

void Galaga::ChangeState(GameState newState)
{
    mCurrentState = newState;
    mStateTimer = 0.0f;
}

void Galaga::Initialize(GameContext* context)
{
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Num5, "CoinInserted");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Num1, "StartPressed");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::A, "MoveLeftDown");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Left, "MoveLeftDown");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::A, "MoveLeftUp");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::Left, "MoveLeftUp");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::D, "MoveRightDown");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Right, "MoveRightDown");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::D, "MoveRightUp");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::Right, "MoveRightUp");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Space, "Shoot");

    std::weak_ptr<GameObject> galagaPtr = shared_from_this();
    context->mNotificationManager->Register(galagaPtr, "CoinInserted");
    context->mNotificationManager->Register(galagaPtr, "StartPressed");
    context->mNotificationManager->Register(galagaPtr, "EnemyDied");
    context->mNotificationManager->Register(galagaPtr, "PlayerDied");
}

void Galaga::ReceiveNotification(const std::string& message)
{
    if (mCurrentState == GameState::WaitingForCoins)
    {
        if (message == "CoinInserted")
        { 
            mCredits++; 
        }
        else if (message == "StartPressed" && mCredits > 0)
        {
            mCredits--;
            ChangeState(GameState::Startup);
        }
    }

    if (message == "EnemyDied")
    {
        mScore += 100;
        mActiveEnemyCount--;
    } else if (message == "PlayerDied")
    {
        mLives--;
        if (auto p = mPlayer.lock())
        {
            std::dynamic_pointer_cast<Player>(p)->SetPaused(true);
        }
        if (mLives <= 0)
        {
            ChangeState(GameState::GameOver);
        }
    }
}

void Galaga::Update(GameContext* context)
{
    float deltaTime = 1.0f / 30.0f;
    mStateTimer += deltaTime;

    mEnemies.erase(std::remove_if(mEnemies.begin(), mEnemies.end(),
    [](const std::weak_ptr<GameObject>& weakPointer) { return weakPointer.expired(); }),
    mEnemies.end());

    switch (mCurrentState)
    {
        case GameState::WaitingForCoins:UpdateWaitingForCoins(context); break;
        case GameState::Startup:UpdateStartup(context); break;
        case GameState::Waves:UpdateWaves(context); break;
        case GameState::Gameplay:UpdateGameplay(context); break;
        case GameState::GameOver:UpdateGameOver(context); break;
    }
}

void Galaga::LateUpdate(GameContext* context) {}

void Galaga::UpdateWaitingForCoins(GameContext* context) {}

void Galaga::UpdateStartup(GameContext* context)
{
    if (mStateTimer <= 1.0f / 30.0f)
    {
        ResetGame(context);

        if (mCurrentLevel == 1)
        {
            auto player = std::make_shared<Player>(CMPUT350::Point2D(768 / 2, 900));
            mPlayer = player;
            context->mEngineView->AddGameObject(player);
            std::weak_ptr<GameObject> playerPtr = shared_from_this();
            context->mNotificationManager->Register(playerPtr, "MoveLeftDown");
            context->mNotificationManager->Register(playerPtr, "MoveLeftUp");
            context->mNotificationManager->Register(playerPtr, "MoveRightDown");
            context->mNotificationManager->Register(playerPtr, "MoveRightUp");
            context->mNotificationManager->Register(playerPtr, "Shoot");
        }
    }
    if (mStateTimer >= 3.0f)
    {
        mCurrentWaveIndex = 0;
        mShipsSpawnedInWave = 0;
        ChangeState(GameState::Waves);
    }
}

void Galaga::UpdateWaves(GameContext* context)
{
    float deltaTime = 1.0f / 30.0f;
    mSpawnTimer += deltaTime;

    // 5 waves of 8 ships
    constexpr int kNumWaves = 5;
    if (mCurrentWaveIndex < kNumWaves)
    {

        if (mShipsSpawnedInWave < 8 && mSpawnTimer >= 0.4f)
        {
            mSpawnTimer = 0.0f;

            // Each wave filles one row; each ship in the wave takes the next column
            int row = mCurrentWaveIndex;
            int col = mShipsSpawnedInWave;

            // Where this ship will sit in the formation
            CMPUT350::Point2D slot = GetFormationSlot(row, col);
            // The curve it flies to get there (ends exactly at the slot)
            std::vector<Point2D> path = GenerateBezierPath(mCurrentWaveIndex, slot);

            // Bosses on the top row, Butterflies in the middle, Bees at the bottom
            std::shared_ptr<Enemy> enemy;
            if (row == 0) {
                enemy = std::make_shared<Boss>(slot, path);
            }
            else if (row <= 2) {
                enemy = std::make_shared<Butterfly>(slot, path);
            }
            else {
                enemy = std::make_shared<Bee>(slot, path);
            }
            
            context->mEngineView->AddGameObject(enemy);
            mEnemies.push_back(enemy);
            mActiveEnemyCount++;
            mShipsSpawnedInWave++;
        }
        if (mShipsSpawnedInWave >= 8)
        {
            mCurrentWaveIndex++;
            mShipsSpawnedInWave = 0;
        }
    }
    else
    {  
        // All ships have spawned; wait until every one has reached its slot
        bool allSeated = true;
        for (auto& weakEnemy: mEnemies) {
            auto enemy = weakEnemy.lock();
            if (enemy && !enemy->IsInFormation()) {
                allSeated = false;
            }
        }
        
        // Only start attacking once the whole formation is in place
        if (allSeated) {
            ChangeState(GameState::Gameplay);
        }
    }
}

void Galaga::UpdateGameplay(GameContext* context)
{
    float deltaTime = 1.0f / 30.0f;
    mAttackCooldown -= deltaTime;
    if (mAttackCooldown <= 0.0f && mActiveEnemyCount > 0)
    {
        mAttackCooldown = 3.5f;
        LaunchAttackingEnemy(context);
    }
    if (mActiveEnemyCount <= 0)
    {
        mCurrentLevel++;
        ChangeState(GameState::Startup);
    }
}

void Galaga::UpdateGameOver(GameContext* context)
{
    if (mStateTimer >= 5.0f)
    {
        ChangeState(GameState::WaitingForCoins);
    }
}

void Galaga::LaunchAttackingEnemy(GameContext* context)
{
    // Gather every enemy that is sitting in formation
    std::vector<std::shared_ptr<Enemy>> candidates;
    for (auto& weakEnemy : mEnemies) {
        auto enemy = weakEnemy.lock();
        if (enemy && enemy->IsAlive() && enemy->IsInFormation()) {
            candidates.push_back(enemy);
        }
    }

    if (candidates.empty()) {
        return;
    }

    // Pick one at random so it isn't always the same enemy
    int index = std::rand() % static_cast<int>(candidates.size());

    // Aim at the player if there is one, otherwise the bottom middle
    CMPUT350::Point2D target(mScreenWidth / 2.0f, mScreenHeight - 60.0f);
    if (auto player = mPlayer.lock()) {
        target = player->GetLocation();
    }

    candidates[index]->Attack(context, target);
    
}

void Galaga::ResetGame(GameContext* context)
{
    mEnemies.clear();
    mActiveEnemyCount = 0;
    if (mCurrentState == GameState::Startup && mCurrentLevel == 1)
    {
        mScore = 0;
        mLives = 3;
    }
}

/*
Output: 7 bezier control points (2 cubic curves joined together). 
The ship drops in from the top, loops through the lower screen,
then rises up and ends exactly at its formation slot
*/
std::vector<Point2D> Galaga::GenerateBezierPath(int waveIndex, CMPUT350::Point2D slot) const
{
    float width = static_cast<float>(mScreenWidth);
    float height = static_cast<float>(mScreenHeight);
    float cx = width / 2.0f;

    // Flip the path left/right on every other wave
    float side = (waveIndex % 2 == 0) ? 1.0f : -1.0f;

    // First curve: drop in from above the screen and swing out to one side
    Point2D p0(cx - side * width * 0.15f, -40.0f);
    Point2D p1(cx - side * width * 0.15f, height * 0.30f);
    Point2D p2(cx + side * width * 0.35f,height * 0.40f);
    Point2D p3(cx + side * width * 0.25f, height * 0.60f);

    // Second curve: loop back and rise up into the slot
    Point2D p4 = p3 + (p3 - p2);
    Point2D p5(slot.x, slot.y + 250.0f);
    Point2D p6 = slot;

    return {p0, p1, p2, p3, p4, p5, p6};
}

void Galaga::RenderUI(GameContext* context)
{
    if (mCurrentState == GameState::WaitingForCoins)
    {
        // The engine clears the window every frame, so only draw the text that matches the current credits
        const char* text = (mCredits > 0) ? "START GAME" : "INSERT COIN";
        context->ScreenContext->DrawText(text, 25, CMPUT350::Point2D(mScreenWidth / 3.0f, mScreenHeight / 2.0f), CMPUT350::Colors::white);
    }

    else if (mCurrentState == GameState::Startup) {
        std::string text = "LEVEL " + std::to_string(mCurrentLevel);
        context->ScreenContext->DrawText(text, 25, CMPUT350::Point2D(mScreenWidth / 3.0f, mScreenHeight / 2.0f), CMPUT350::Colors::green);
    }
}

CMPUT350::Point2D Galaga::GetFormationSlot(int row, int col) const {
    constexpr int kCols = 8;
    constexpr float kSpacingX = 60.0f;
    constexpr float kSpacingY = 50.0f;
    constexpr float kTopY = 120.0f;

    float gridWidth = (kCols - 1) * kSpacingX;
    float left = mScreenWidth / 2.0f - gridWidth / 2.0f;
    return CMPUT350::Point2D(left + col * kSpacingX, kTopY + row * kSpacingY);
}