#include "Galaga.h"
#include <Keyboard.h>
#include <GameObject.h>
#include <DrawContext.h>

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
        { mCredits++; }
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

    if (mCurrentWaveIndex < 5)
    {
        if (mShipsSpawnedInWave < 8 && mSpawnTimer >= 0.4f)
        {
            mSpawnTimer = 0.0f;
            //std::vector<CMPUT350::Line> path = GenerateBezierPath(mCurrentWaveIndex);
            auto enemy = std::make_shared<Enemy>(CMPUT350::Point2D(mScreenWidth/2, mScreenHeight/2));

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
            enemy->StartAttackRun();
            break;
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

std::vector<CMPUT350::Line> Galaga::GenerateBezierPath(int waveIndex)
{
    float width = static_cast<float>(mScreenWidth);
    float height = static_cast<float>(mScreenHeight);
    return {
        { width * 0.0f, height * 0.0f },
        { width * 0.5f, height * 0.5f },
        { width * 1.0f, height * 1.0f }
    };
}

void Galaga::RenderUI(GameContext* context)
{
    if (mCurrentState == GameState::WaitingForCoins)
    {
        context->ScreenContext->DrawText("INSERT COIN", 25, CMPUT350::Point2D(mScreenWidth / 3, mScreenHeight / 2), CMPUT350::Colors::white);
    }
}