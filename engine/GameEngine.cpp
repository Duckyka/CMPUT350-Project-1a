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
        //std::cout << "Test in GameEngine, in Init First: " << "\n";
        //std::cout << "Test in GameEngine, in Init Second: " << "\n";
        mWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), name);
        mWindow.get()->setFramerateLimit(30);
        
        // mWindow.reset(new sf::RenderWindow(sf::VideoMode({width, height}), name));
        
        // Load the font from embedded data
        mFont = std::make_shared<sf::Font>();
        mFont->openFromMemory(_font, sizeof(_font));
        std::cout << "Test in GameEngine, in Init Third: " << "\n";

        mDrawContext = std::make_shared<DrawContext>(mWindow, mFont);
        
    }

    GameEngine::~GameEngine() {
        // Cleanup resources
        // while (waitingObjects.size() != 0)
        // {
        //     waitingObjects.pop_back();
        // }
        // while (activeObjects.size() != 0)
        // {
        //     activeObjects.back().Kill();
        //     activeObjects.back().~GameObject();
        //     activeObjects.pop_back();
        // }
        // mWindow->close();
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
            std::cout << "Testing Run()\n";
            //std::cout << "ActiveList: " << activeObjects.data() << "\n";
            //std::cout << "WaitingList: " << waitingObjects.data() << "\n";
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
                    std::cout << "Key Pressed\n";
                    // TODO: dispatch to game objects' HandleKeyEvent
                    // if (keyPressed->unicode == 'a')
                    // {
                    //     std::cout << "Pressed a\n";
                    // }
                    // else if (keyPressed->unicode == 'd')
                    // {
                    //     std::cout << "Pressed d\n";
                    // }
                    // else if (keyPressed->unicode == ' ')
                    // {
                    //     std::cout << "Pressed space\n";
                    // }
                    for (GameObject object: activeObjects)
                    {
                        object.HandleKeyEvent(&context, keyPressed->unicode);
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

            // std::vector<CollisionObject> tempCollisionObjects;
            // tempCollisionObjects.clear();
            // for (GameObject object: activeObjects)
            // {
            //     std::shared_ptr<CollisionObject> objA = std::dynamic_pointer_cast<CollisionObject>(object);
            //     if (objA == nullptr)
            //     {
            //         continue; //Not a collision object, therefore skip. 
            //     }
            //     else    //It is a collision object, check for collisions
            //     {
            //         tempCollisionObjects.push_back(objA);
            //     }
            // }

            // for (CollisionObject object1: tempCollisionObjects)
            // {
            //     for (CollisionObject object2: tempCollisionObjects)
            //     {
            //         if (object1 != object2)
            //         {
            //             // CHECK FOR COLLISIONS BY CHECKING IF THEY DON'T COLLIDE
            //             //if (
            //             //      (r1.rect.topLeft.x + r1.rect.width) < r2.rect.topLeft.x ||      (if r1 is left of r2)
            //             //      r1.rect.topLeft.x > (r2.rect.topLeft.x + r2.rect.width) ||      (if r1 is right of r2)
            //             //      (r1.rect.topLeft.y + r1.rect.height) < r2.rect.topLeft.x ||     (if r1 is to the top of r2)
            //             //      r1.rect.topleft.y > (r2.rect.topleft.y + r2.rect.height)        (if r1 is to the bottom of r2)
            //             //    )
            //             // DO SOMETHING IF THEY DO COLLIDE
            //         }
            //     }
            // }

            // 5. Late updates
            for (auto &object: mGameObjects)
            {
                object->LateUpdate(&context);
            }

            // Clear window
            mWindow->clear();

            // 6. Render background
            // Step 6: render background
            for (auto &obj : mGameObjects)
            {
                auto g = std::dynamic_pointer_cast<GraphicsObject>(obj);
                if (g) g->RenderBackground(&context);
            }

            // Step 7: render foreground
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
