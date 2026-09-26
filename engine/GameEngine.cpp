#include "GameEngine.h"
#include <vector>

/// @brief
namespace CMPUT350 {
#include "FontData.h"

GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
    // Sample font loading code
    //	if (!mFont->openFromMemory(&_font, _font_len))
    //	{
    //		fprintf(stderr, "WARNING: Font did not load.\n");
    //	}
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
        *activeObjects.back() = nullptr;
        activeObjects.pop_back();
    }
    // mWindow->close();
    mWindow->close();
}

void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
    waitingObjects.push_back(gameObject);
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
        // 0. Remove any objects that are now dead
        int currentSize = activeObjects.size();
        for (int i = 0; i < currentSize; i++)
        {
            if (!activeObjects[i].IsAlive())
            {
                activeObjects.erase(i);
            }
        }

        // 1. Activate and initialize any objects added during the last frame
        while (waitingObjects.size() != 0)
        {
            GameObject temp = waitingObjects.back();
            waitingObjects.pop_back();
            temp(GameContext);                              //Not sure what gameContext to pass to initialize gameObjects--------
            activeObjects.push_back(temp);
        }

        // 2. Process events
        while (const std::optional event = mWindow->pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                this->~GameEngine();
            }
            else if (event->is<sf::Event::Resized>())
            {
                continue;
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::TextEntered>())
            {
                continue;
            }
        }

        // 3. Update game objects
        for (GameObject object: activeObjects)
        {
            object.Update(GameContext);
        }

        // 4. Process collision events
        std::vector<CollisionObject> tempCollisionObjects;
        tempCollisionObjects.clear();
        for (GameObject object: activeObjects)
        {
            std::shared_ptr<CollisionObject> objA = std::dynamic_pointer_cast<CollisionObject>(object);
            if (objA == nullptr)
            {
                continue; //Not a collision object, therefore skip. 
            }
            else    //It is a collision object, check for collisions
            {
                tempCollisionObjects.push_back(objA);
            }
        }

        for (CollisionObject object1: tempCollisionObjects)
        {
            for (CollisionObject object2: tempCollisionObjects)
            {
                if (object1 != object2)
                {
                    // CHECK FOR COLLISIONS BY CHECKING IF THEY DON'T COLLIDE
                    //if (
                    //      (r1.rect.topLeft.x + r1.rect.width) < r2.rect.topLeft.x ||      (if r1 is left of r2)
                    //      r1.rect.topLeft.x > (r2.rect.topLeft.x + r2.rect.width) ||      (if r1 is right of r2)
                    //      (r1.rect.topLeft.y + r1.rect.height) < r2.rect.topLeft.x ||     (if r1 is to the top of r2)
                    //      r1.rect.topleft.y > (r2.rect.topleft.y + r2.rect.height)        (if r1 is to the bottom of r2)
                    //    )
                    // DO SOMETHING IF THEY DO COLLIDE
                }
            }
        }

        // 5. Late updates
        for (GameObject object: activeObjects)
        {
            object.LateUpdate(GameContext);
        }

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

private:
    std::vector<GameObject> testVector;             //------------------------------------------------------------
    //Not sure why, the first vector after private is not highlighting correctly for me, put this here to ensure both vectors work

    std::vector<GameObject> activeObjects;
    std::vector<GameObject> waitingObjects;

}  // namespace CMPUT350
