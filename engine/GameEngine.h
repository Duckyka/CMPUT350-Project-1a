
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

namespace CMPUT350 {
class GameEngine;
}

#include "EngineView.h"
#include "GameObject.h"
#include "MathUtil.h"
#include <SFML/Graphics.hpp>
//Is This Allowed????
#include "GameContext.h"

namespace CMPUT350 {

class DrawContext;

class GameEngine : public EngineView {
public:
    std::vector<GameObject> activeObjects;
    std::vector<GameObject> waitingObjects;
    GameEngine(unsigned int width, unsigned int height, const std::string& name);
    ~GameEngine();

    GameEngine(const GameEngine&) = delete;             // Prevent copy-construction
    GameEngine(GameEngine&&) = delete;                  // Prevent move-construction
    GameEngine& operator=(const GameEngine&) = delete;  // Prevent assignment
    GameEngine& operator=(GameEngine&&) = delete;       // Prevent move-assignment

    void AddGameObject(std::shared_ptr<GameObject> gameObject) override;

    void Run();

private:
    std::shared_ptr<sf::RenderWindow> mWindow;
    std::shared_ptr<sf::Font> mFont;
    GameContext mContext;
};

}  // namespace CMPUT350

#endif  // GAMEENGINE_H
