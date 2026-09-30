#include "GameEngine.h"
#include "DrawContext.h"
#include "FontData.h"
#include "GraphicsObject.h"
#include "GameContext.h"
#include "CollisionObject.h"
#include <algorithm>
#include <vector>

/// @brief
namespace CMPUT350 {

    GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
        // Sample font loading code
        mWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), name);
        mWindow.get()->setFramerateLimit(30);
                
        // Load the font from embedded data
        mFont = std::make_shared<sf::Font>();
        mFont->openFromMemory(_font, sizeof(_font));

        mDrawContext = std::make_shared<DrawContext>(mWindow, mFont);
        
    }

    GameEngine::~GameEngine() {
        // Cleanup resources
        mWindow->close();
    }

    void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject)
    {
        mPendingGameObjects.push_back(gameObject);
    }

/**
 * @method Run
 * @arguments None
 * @description Gives control to the game engine. Will not return until the game window is closed or
 * all objects have been destroyed.
    */
    void GameEngine::Run() {
        GameContext context;
        context.mEngineView = this;
        context.ScreenContext = mDrawContext.get();

        while (mWindow->isOpen())  // window is open
        {
            // 0. Remove any objects that are now dead
            for (int i = static_cast<int>(mGameObjects.size()) - 1; i >= 0; --i) {
                if (!mGameObjects[i]->IsAlive())
                {
                    mGameObjects.erase(mGameObjects.begin() + i);
                }
            }

            // 1. Activate and initialize any objects added during the last frame
            for (auto &obj : mPendingGameObjects) {
                obj->Initialize(&context);
                mGameObjects.push_back(obj);
            }
            mPendingGameObjects.clear();

            // 2. Process events
            while (const std::optional event = mWindow->pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                {
                   mWindow->close();
                }
                else if (const auto* keyPressed = event->getIf<sf::Event::TextEntered>()) {
                    // TODO: dispatch to game objects' HandleKeyEvent
                    for (auto &object: mGameObjects)
                    {
                        object->HandleKeyEvent(&context, keyPressed->unicode);
                    }
                }
            }

            // 3. Update game objects
            for (auto object: mGameObjects)
            {
                object->Update(&context);
            }

            // 4. Process collision events
            std::vector<std::shared_ptr<CollisionObject>> collidables;
            for (auto &obj : mGameObjects)
            {
                auto c = std::dynamic_pointer_cast<CollisionObject>(obj);
                if (c) collidables.push_back(c);
            }
            for (size_t i = 0; i < collidables.size(); ++i)
            {
                for (size_t j = i + 1; j < collidables.size(); ++j)
                {
                    const Rect &r1 = collidables[i]->GetBounds();
                    const Rect &r2 = collidables[j]->GetBounds();
                    bool overlap = !(
                        (r1.topLeft.x + r1.width) < r2.topLeft.x ||
                        r1.topLeft.x > (r2.topLeft.x + r2.width) ||
                        (r1.topLeft.y + r1.height) < r2.topLeft.y ||
                        r1.topLeft.y > (r2.topLeft.y + r2.height)
                    );
                    if (overlap)
                    {
                        collidables[i]->CollisionEnter(collidables[j]);
                        collidables[j]->CollisionEnter(collidables[i]);
                    }
                }
            }

            // 5. Late updates
            for (auto &object: mGameObjects)
            {
                object->LateUpdate(&context);
            }

            // Clear window
            mWindow->clear();

            // 6. Render background
            for (auto &obj : mGameObjects)
            {
                auto g = std::dynamic_pointer_cast<GraphicsObject>(obj);
                if (g) g->RenderBackground(&context);
            }

            // 7. Render foreground
            for (auto &obj : mGameObjects)
            {
                auto g = std::dynamic_pointer_cast<GraphicsObject>(obj);
                if (g) g->RenderForeground(&context);
            }

            // Actually render to window
            mWindow->display();
        }
    }

// Sample code for processing events

// bool GameEngine::ProcessEvents(GameContext *context)
//{
//	while (const std::optional event = mWindow->pollEvent())
//	{
//		if (event->is<sf::Event::Closed>())
//		{
//		}
//		else if (event->is<sf::Event::Resized>())
//		{
//		}
//		else if (const auto* keyPressed = event->getIf<sf::Event::TextEntered>())
//		{
//			// use keyPressed->unicode to get character
//		}
//	}
// }

}  // namespace CMPUT350
