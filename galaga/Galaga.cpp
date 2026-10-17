#include "Galaga.h"
#include "Bee.h"
#include "Butterfly.h"
#include "Boss.h"
#include "Stars.h"
#include <Keyboard.h>
#include <GameObject.h>
#include <DrawContext.h>
#include <memory>
#include <string>
#include "Bezier.h"
#include "MathUtil.h"
#include <unordered_map>

namespace {
    // Points for each enemy death message.
    // Diving enemies are worth more than enemies in formation.
    const std::unordered_map<std::string, int> kDeathPoints = {
        {"BeeDiedFormation", 50},        {"BeeDiedDiving", 100},
        {"ButterflyDiedFormation", 80},  {"ButterflyDiedDiving", 160},
        {"BossDiedFormation", 150},      {"BossDiedDiving", 400},
    };
}

/*
Create the game manager. The game starts waiting for a coin.
*/
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
    mAttackCooldown(3.0f)
{
}

Galaga::~Galaga() {}

/*
Output: true. The game manager stays in the engine for the whole program.
*/
bool Galaga::IsAlive() const
{
    return mIsAlive;
}

/*
Switch to a new state and restart the state timer.
*/
void Galaga::ChangeState(GameState newState)
{
    mCurrentState = newState;
    mStateTimer = 0.0f;
}

/*
Called once when Galaga is added to the engine.
Creates the stars, sets up the keys, and registers for the messages Galaga needs.
*/
void Galaga::Initialize(GameContext* context)
{
    // Star background, stays for the whole game
    CMPUT350::Rect screen(0.0f, 0.0f, static_cast<float>(mScreenWidth), static_cast<float>(mScreenHeight));
    context->mEngineView->AddGameObject(std::make_shared<Stars>(250, screen));

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
    // Listen for every kind of enemy death
    for (const auto& entry : kDeathPoints) {
        context->mNotificationManager->Register(galagaPtr, entry.first);
    }
    context->mNotificationManager->Register(galagaPtr, "PlayerDied");
}

/*
Handle coins, the start button, enemy deaths (score) and player deaths (lives).
*/
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

    bool playing = (mCurrentState == GameState::Waves || mCurrentState == GameState::Gameplay);
    // Look up how many points this enemy death is worth
    auto points = kDeathPoints.find(message);
    if (points != kDeathPoints.end() && playing)
    {
        mScore += points->second;
        mActiveEnemyCount--;
    }
    else if (message == "PlayerDied" && playing)
    {
        mLives--;
        if (mLives <= 0)
        {
            ClearBoard();
            ChangeState(GameState::GameOver);
        }
        else {
            // Wait for every enemy to return home before respawning
            mWaitingToRespawn = true;
            mRespawnTimer = 0.0f;
        }
    }
}

/*
Called every frame. Runs the update for the current state.
*/
void Galaga::Update(GameContext* context)
{
    float deltaTime = 1.0f / 30.0f;
    mStateTimer += deltaTime;

    // Forget enemies that have been removed from the engine
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

// Nothing to do after collisions
void Galaga::LateUpdate(GameContext* context) {}

// Waiting for a coin: everything happens in ReceiveNotification
void Galaga::UpdateWaitingForCoins(GameContext* context) {}

/*
Startup: on the first frame, reset the level and spawn the player.
After 3 seconds of showing the level number, start the waves.
*/
void Galaga::UpdateStartup(GameContext* context)
{
    if (mStateTimer <= 1.0f / 30.0f)
    {
        ResetGame(context);

        if (mPlayer.expired()) {
            SpawnPlayer(context);
        }
    }
    if (mStateTimer >= 3.0f)
    {
        mCurrentWaveIndex = 0;
        mShipsSpawnedInWave = 0;
        ChangeState(GameState::Waves);
    }
}

/*
Waves: spawn 5 waves of 8 ships, one ship every 0.4 seconds.
Once every ship is in formation, move on to Gameplay.
*/
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

            // Each wave fills one row; each ship in the wave takes the next column
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
        // All ships have spawned; start attacking once the whole formation is in place
        if (AllEnemiesInFormation()) {
            ChangeState(GameState::Gameplay);
        }
    }
}

/*
Gameplay: send enemies to attack every few seconds.
Goes back to Startup when every enemy is destroyed.
If the player died, wait for enemies to return home, then respawn the player.
*/
void Galaga::UpdateGameplay(GameContext* context)
{
    float deltaTime = 1.0f / 30.0f;

    // Level cleared: go to next level
    if (mActiveEnemyCount <= 0) {
        mCurrentLevel++;
        ChangeState(GameState::Startup);
        return;
    }

    if (mWaitingToRespawn) {
        // No new attacks while the player is dead, wait for divers to return home
        if (!AllEnemiesInFormation()) {
            return;
        }

        // Everyone is home: show "READY" for 2 seconds
        mRespawnTimer += deltaTime;
        if (mRespawnTimer >= 2.0f) {
            SpawnPlayer(context);
            mWaitingToRespawn = false;
            mAttackCooldown = 3.0f;
        }
        return;
    }

    mAttackCooldown -= deltaTime;
    if (mAttackCooldown <= 0.0f)
    {
        mAttackCooldown = 3.5f;
        LaunchAttackingEnemy(context);
    }
}

/*
Game over: show the message for 5 seconds, then wait for a new coin.
*/
void Galaga::UpdateGameOver(GameContext* context)
{
    if (mStateTimer >= 5.0f)
    {
        mCurrentLevel = 1;
        mWaitingToRespawn = false;
        ChangeState(GameState::WaitingForCoins);
    }
}

/*
Send up to 2 enemies from the formation to attack the player.
*/
void Galaga::LaunchAttackingEnemy(GameContext* context)
{
    // Gather every enemy in the engine that is sitting in formation
    std::vector<std::shared_ptr<Enemy>> candidates;
    for (const auto& object : *context->mEngineView) {
        auto enemy = std::dynamic_pointer_cast<Enemy>(object);
        if (enemy && enemy->IsAlive() && enemy->IsInFormation()) {
            candidates.push_back(enemy);
        }
    }

    if (candidates.empty()) {
        return;
    }

    // Aim at the player if there is one, otherwise the bottom middle
    CMPUT350::Point2D target(mScreenWidth / 2.0f, mScreenHeight - 60.0f);
    if (auto player = mPlayer.lock()) {
        target = player->GetLocation();
    }

    // Pick up to 2 enemies in a fixed order. The counter moves the starting
    // point each attack, and the second pick is half the list away.
    int count = static_cast<int>(candidates.size());
    int numToLaunch = std::min(2, count);
    for (int k = 0; k < numToLaunch; k++) {
        int index = (mAttackCounter + k * (count / 2)) % count;
        candidates[index]->Attack(context, target);
    }
    mAttackCounter++;
}

/*
Clear per-level state. On level 1 (a new game) also reset score and lives.
Called once at the start of Startup.
*/
void Galaga::ResetGame(GameContext* context)
{
    mEnemies.clear();
    mActiveEnemyCount = 0;
    mSpawnTimer = 0.0f;
    mAttackCooldown = 3.0f;
    mWaitingToRespawn = false;

    if (mCurrentLevel == 1)
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

/*
Draw the on-screen text for the current game state.
Uses the GUI context, so nothing here is moved or rotated by the engine.
*/
void Galaga::RenderUI(GameContext* context)
{
    auto* gui = context->GUIContext;
    // Center of the screen; messages here use DrawCenteredText so they line up
    CMPUT350::Point2D middle(mScreenWidth / 2.0f, mScreenHeight / 2.0f);

    if (mCurrentState == GameState::WaitingForCoins)
    {
        // The engine clears the window every frame, so only draw the text that matches the current credits
        const char* text = (mCredits > 0) ? "PUSH 1 TO START" : "INSERT COIN";
        gui->DrawCenteredText(text, 25, middle, CMPUT350::Colors::white);
        gui->DrawText("CREDIT " + std::to_string(mCredits), 20, CMPUT350::Point2D(30, mScreenHeight - 40.0f), CMPUT350::Colors::white);
        return;
    }

    // Once a game has started, the score is always on screen
    // Label in green, number in white. The engine can't measure text width,
    // so the number is drawn a fixed distance to the right of the label.
    CMPUT350::Point2D scoreLoc(mScreenWidth / 2.5f, 20);
    gui->DrawText("SCORE", 25, scoreLoc, CMPUT350::Colors::green);
    gui->DrawText(std::to_string(mScore), 25, scoreLoc + CMPUT350::Point2D(kScoreNumberOffset, 0.0f), CMPUT350::Colors::white);

    // Lives tracker: one small ship per spare life (the ship in play isn't counted)
    if (mCurrentState != GameState::GameOver)
    {
        constexpr float kIconSpacing = 45.0f;
        for (int i = 0; i < mLives - 1; i++)
        {
            CMPUT350::Point2D center(35.0f + i * kIconSpacing, mScreenHeight - 35.0f);
            DrawLifeIcon(gui, center);
        }
    }

    // Message for the current state
    if (mCurrentState == GameState::Startup)
    {
        gui->DrawCenteredText("LEVEL " + std::to_string(mCurrentLevel), 25, middle, CMPUT350::Colors::green);
    }
    else if (mCurrentState == GameState::Gameplay && mWaitingToRespawn && AllEnemiesInFormation())
    {
        // Tell the player how many ships they have left, counting the one about to spawn
        gui->DrawCenteredText("READY", 25, middle, CMPUT350::Colors::red);
        std::string livesText = std::to_string(mLives) + (mLives == 1 ? " SHIP LEFT" : " SHIPS LEFT");
        gui->DrawCenteredText(livesText, 20, middle + CMPUT350::Point2D(0.0f, 40.0f), CMPUT350::Colors::white);
    }
    else if (mCurrentState == GameState::GameOver)
    {
        gui->DrawCenteredText("GAME OVER", 30, middle, CMPUT350::Colors::red);
    }
}

/*
Get the row and column of the slot that the enemy is supposed to get into
*/
CMPUT350::Point2D Galaga::GetFormationSlot(int row, int col) const {
    constexpr int kCols = 8;
    constexpr float kSpacingX = 60.0f;
    constexpr float kSpacingY = 50.0f;
    constexpr float kTopY = 120.0f;

    float gridWidth = (kCols - 1) * kSpacingX;
    float left = mScreenWidth / 2.0f - gridWidth / 2.0f;
    return CMPUT350::Point2D(left + col * kSpacingX, kTopY + row * kSpacingY);
}

/*
Create a new player ship at the bottom of the screen.
The Player registers for its own key notifications in
Player::Initialize.
*/
void Galaga::SpawnPlayer(GameContext* context)
{
    auto player = std::make_shared<Player>(CMPUT350::Point2D(mScreenWidth / 2.0f, mScreenHeight - 124.0f));
    mPlayer = player;
    context->mEngineView->AddGameObject(player);
}

/*
Output: true if every living enemy is sitting in its formation slot
*/
bool Galaga::AllEnemiesInFormation() const {
    for (auto& weakEnemy : mEnemies) {
        auto enemy = weakEnemy.lock();
        if (enemy && enemy->IsAlive() && !enemy->IsInFormation()) {
            return false;
        }
    }
    return true;
}

/*
Remove the player and every enemy from the engine (used on game over)
Killed objects are removed by the engine at the start of the next frame
*/
void Galaga::ClearBoard() {
    if (auto player = mPlayer.lock()) {
        player->Kill();
    }

    for (auto& weakEnemy : mEnemies) {
        if (auto enemy = weakEnemy.lock()) {
            enemy->Kill();
        }
    }
    mEnemies.clear();
    mActiveEnemyCount = 0;
}
/*
Draw a small copy of the player ship (half size) centered on center.
Used for the lives tracker in the bottom left corner.
*/
void Galaga::DrawLifeIcon(CMPUT350::DrawContext* gui, CMPUT350::Point2D center) const
{
    // Same rectangles as Player (body, wings, nose), scaled by 0.5
    gui->DrawRect(CMPUT350::Rect(-10.0f, -10.0f, 20.0f, 20.0f) + center, CMPUT350::Colors::blue);
    gui->DrawRect(CMPUT350::Rect(-17.5f, 2.5f, 35.0f, 6.0f) + center, CMPUT350::Colors::blue);
    gui->DrawRect(CMPUT350::Rect(-3.0f, -17.5f, 6.0f, 29.0f) + center, CMPUT350::Colors::blue);
}
