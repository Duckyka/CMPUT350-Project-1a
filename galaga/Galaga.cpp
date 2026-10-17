#include "Galaga.h"
#include "Bee.h"
#include "Butterfly.h"
#include "Boss.h"
#include <Keyboard.h>
#include <GameObject.h>
#include <DrawContext.h>
#include <string>
#include "Bezier.h"

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

    // TESTING: only 1 wave so attacks start sooner. Set back to 5 for the full game.
    constexpr int kNumWaves = 1;
    if (mCurrentWaveIndex < kNumWaves)
    {

        if (mShipsSpawnedInWave < 8 && mSpawnTimer >= 0.4f)
        {
            mSpawnTimer = 0.0f;
            std::vector<Point2D> path = GenerateBezierPath(mCurrentWaveIndex, mShipsSpawnedInWave);
            CMPUT350::Point2D spawnLocation({mScreenWidth * 0.5f, mScreenHeight * 0.5f});
            std::shared_ptr<Enemy> enemy;
            switch (mShipsSpawnedInWave % 3)
            {
                case 0:  enemy = std::make_shared<Bee>(spawnLocation); break;
                case 1:  enemy = std::make_shared<Butterfly>(spawnLocation); break;
                default: enemy = std::make_shared<Boss>(spawnLocation); break;
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
        ChangeState(GameState::Gameplay);
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
    for (auto iterator = context->mEngineView->begin(); iterator != context->mEngineView->end(); iterator++)
    {
        if (auto enemy = std::dynamic_pointer_cast<Enemy>(*iterator))
        {
            // test target: bottom-right corner of the screen
            CMPUT350::Point2D target(mScreenWidth - 60.0f, mScreenHeight - 60.0f);
            if (enemy->Attack(context, target)) {
                break;
            }
            // enemy->StartAttackRun();
            // break;
        }
    }
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

std::vector<Point2D> Galaga::GenerateBezierPath(int waveIndex, int currShipsInWave)
{
    float width = static_cast<float>(mScreenWidth);
    float height = static_cast<float>(mScreenHeight);

    float shipWidth = (width - 20) / 10 * currShipsInWave;
    float shipHeight = 20 * waveIndex;

    if (mCurrentLevel % 5 == 1)
    {
        std::vector<Point2D> points = {{ width * 0.0f, height * 0.0f },{ width * 0.2f, height * 0.2f },{ width * 0.8f, height * 0.2f },{ shipWidth, shipHeight }};
        return Bezier(points).GetCurve();
    }
    else if (mCurrentLevel % 5 == 2)
    {
        std::vector<Point2D> points = {{ width * 0.0f, height * 0.5f },{ width * 0.65f, height * 0.40f },{ width * 0.30f, height * 0.30f },{ shipWidth, shipHeight }};
        return Bezier(points).GetCurve();
    }
    else if (mCurrentLevel % 5 == 3)
    {
        std::vector<Point2D> points = {{ width * 0.0f, height * 0.0f },{ width * 0.2f, height * 0.2f },{ width * 0.8f, height * 0.2f },{ shipWidth, shipHeight }};
        return Bezier(points).GetCurve();
    }
    else if (mCurrentLevel % 5 == 4)
    {
        std::vector<Point2D> points = {{ width * 0.0f, height * 0.0f },{ width * 0.2f, height * 0.2f },{ width * 0.8f, height * 0.2f },{ shipWidth, shipHeight }};
        return Bezier(points).GetCurve();
    }
    else
    {
        std::vector<Point2D> points = {{ width * 0.0f, height * 0.0f },{ width * 0.2f, height * 0.2f },{ width * 0.8f, height * 0.2f },{ shipWidth, shipHeight }};
        return Bezier(points).GetCurve();
    }
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