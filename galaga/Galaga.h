#ifndef GALAGA_H
#define GALAGA_H

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <GameObject.h>
#include <GameEngine.h>
#include <Player.h>
#include <Enemy.h>
#include <GameContext.h>

using GameContext = CMPUT350::GameContext;

enum class GameState
{
    WaitingForCoins,
    Startup,
    Waves,
    Gameplay,
    GameOver
};

class Galaga : public CMPUT350::GameObject, public std::enable_shared_from_this<Galaga>
{
public:
    Galaga(size_t screenWidth, size_t screenHeight);
    virtual ~Galaga() override;

    virtual void Initialize(GameContext* context) override;
    virtual void Update(GameContext* context) override;
    virtual void LateUpdate(GameContext* context) override;
    virtual void RenderUI(GameContext* context) override;
    virtual bool IsAlive() const override;

    void ReceiveNotification(const std::string& message) override;
    
private:
    void ChangeState(GameState newState);

    void UpdateWaitingForCoins(GameContext* context);
    void UpdateStartup(GameContext* context);
    void UpdateWaves(GameContext* context);
    void UpdateGameplay(GameContext* context);
    void UpdateGameOver(GameContext* context);

    void ResetGame(GameContext* context);
    void LaunchAttackingEnemy(GameContext* context);
    std::vector<CMPUT350::Point2D> GenerateBezierPath(int waveIndex, CMPUT350::Point2D slot) const;
    CMPUT350::Point2D GetFormationSlot(int row, int col) const;
    void SpawnPlayer(GameContext* context);
    bool AllEnemiesInFormation() const;
    void ClearBoard();
    void DrawLifeIcon(CMPUT350::DrawContext* gui, CMPUT350::Point2D center) const;

    size_t mScreenWidth;
    size_t mScreenHeight;

    bool mIsAlive;
    GameState mCurrentState;
    float mStateTimer;

    int mCredits;
    int mScore;
    int mLives;
    int mCurrentLevel;
    
    int mCurrentWaveIndex;
    int mShipsSpawnedInWave;
    float mSpawnTimer;
    int mActiveEnemyCount;

    std::weak_ptr<Player> mPlayer;
    std::vector<std::weak_ptr<Enemy>> mEnemies;

    float mAttackCooldown;
    // Counts attacks so attackers are picked in a fixed order
    int mAttackCounter = 0;
    bool mWaitingToRespawn = false;
    float mRespawnTimer = 0.0f;
    static constexpr float kScoreNumberOffset = 160.0f;
};

#endif //GALAGA_H