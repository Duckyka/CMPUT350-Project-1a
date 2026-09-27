#include "GameEngine.h"

/// @brief
namespace CMPUT350 {
    #include "FontData.h"
    #include <vector>

    GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
        // Sample font loading code
        std::cout << "Test in GameEngine, in Init First: " << "\n";
        // if (!mFont->openFromMemory(&_font, _font_len))
        // {
        //     std::cout << "FONT NOT LOADED\n";
        //     fprintf(stderr, "WARNING: Font did not load.\n");
        // }
        std::cout << "Test in GameEngine, in Init Second: " << "\n";
        mWindow.reset(new sf::RenderWindow(sf::VideoMode({width, height}), name));
        mWindow.get()->setFramerateLimit(30);
        std::cout << "Test in GameEngine, in Init Third: " << "\n";
        DrawContext mDraw(mWindow, mFont);
        *mContext.ScreenContext = mDraw;
    }

    GameEngine::~GameEngine() {
        // Cleanup resources
        while (waitingObjects.size() != 0)
        {
            waitingObjects.pop_back();
        }
        while (activeObjects.size() != 0)
        {
            activeObjects.back().Kill();
            activeObjects.back().~GameObject();
            activeObjects.pop_back();
        }
        // mWindow->close();
        mWindow->close();
    }

    void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject)
    {
        waitingObjects.push_back(*gameObject);
    }

/**
 * @method Run
 * @arguments None
 * @description Gives control to the game engine. Will not return until the game window is closed or
 * all objects have been destroyed.
    */
    void GameEngine::Run() {
        while (true)  // window is open
        {
            std::cout << "Testing Run()\n";
            std::cout << "ActiveList: " << activeObjects.data() << "\n";
            std::cout << "WaitingList: " << waitingObjects.data() << "\n";
            // 0. Remove any objects that are now dead
            int currentSize = activeObjects.size();
            for (int i = currentSize - 1; i >= 0; i--)
            {
                if (!activeObjects[i].IsAlive())
                {
                    activeObjects.erase(activeObjects.begin() + i);
                }
            }

            // 1. Activate and initialize any objects added during the last frame
            // while (waitingObjects.size() != 0)
            // {
            //     GameObject temp = waitingObjects.back();
            //     waitingObjects.pop_back();
            //     temp(mContext);                              //Not sure what gameContext to pass to initialize gameObjects--------
            //     activeObjects.push_back(temp);
            // }

            // 2. Process events
            // while (const std::optional event = mWindow->pollEvent())
            // {
            //     if (event->is<sf::Event::Closed>())
            //     {
            //         this->~GameEngine();
            //     }
            //     else if (event->is<sf::Event::Resized>())
            //     {
            //         continue;
            //     }
            //     else if (const auto* keyPressed = event->getIf<sf::Event::TextEntered>())
            //     {
            //         continue;
            //     }
            // }

            // 3. Update game objects
            // for (GameObject object: activeObjects)
            // {
            //     object.Update(GameContext);
            // }

            // 4. Process collision events
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
            // for (GameObject object: activeObjects)
            // {
            //     object.LateUpdate(GameContext);
            // }

            // Clear window
            // mWindow->clear();

            // 6. Render background
            // std::vector<CollisionObject> tempGraphicsObjects;
            // tempGraphicsObjects.clear();
            // for (GameObject object: activeObjects)
            // {
            //     std::shared_ptr<GraphicsObject> objA = std::dynamic_pointer_cast<GraphicsObject>(object);
            //     if (objA == nullptr)
            //     {
            //         continue; //Not a graphics object, therefore skip. 
            //     }
            //     else    //It is a graphics object, render to the screen
            //     {
            //         tempGraphicsObjects.push_back(objA);
            //     }
            // }
            // for (GraphicsObject object: tempGraphicsObjects)
            // {
            //     object.RenderBackground();
            // }

            // 7. Render foreground
            // for (GraphicsObject object: tempGraphicsObjects)
            // {
            //     object.RenderForeground();
            // }

            // Actually render to window
            //mWindow->display();

            break;
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
