#include "GameEngine.h"
#include "DrawContext.h"
#include "FontData.h"
#include "GraphicsObject.h"
#include "GameContext.h"
#include "CollisionObject.h"
#include "NotificationManager.h"
#include "Keyboard.h"
#include <algorithm>
#include <vector>

/// @brief
namespace CMPUT350 {

    //Initializer, creates RenderWindow, DrawContext and loads Font from memory
    GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
        // Sample font loading code
        mWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), name);
        mWindow->setFramerateLimit(30);
        
        // mWindow.reset(new sf::RenderWindow(sf::VideoMode({width, height}), name));
        
        // Load the font from embedded data
        mFont = std::make_shared<sf::Font>();
        if (!mFont->openFromMemory(_font, sizeof(_font))) {
            std::cerr << "Failed to load font\n";
        }

        mDrawContext = std::make_shared<DrawContext>(mWindow, mFont);
        mGUIContext = std::make_shared<DrawContext>(mWindow, mFont);
        mWindow->setKeyRepeatEnabled(false);
    }

    //Destructor for GameEngine
    GameEngine::~GameEngine() {
        // Cleanup resources
        mWindow->close();
    }

    //Adds GameObjects to a list which will be initialized and added upon the next iteration of the game loop in Run()
    void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject)
    {
        mPendingGameObjects.push_back(gameObject);
    }

    void GameEngine::InstallKeyDownNotification(int keyCode, const std::string &notification)
    {
        std::cout << "KeyCode, String: " << keyCode << ", " << notification << "\n";
        mKeyDownNotifications[keyCode].push_back(notification);

    }

    void GameEngine::InstallKeyUpNotification(int keyCode, const std::string &notification)
    {
        mKeyUpNotifications[keyCode].push_back(notification);
    }

    GameEngine::const_iterator GameEngine::cbegin() const
    {
        return this->mGameObjects.cbegin();
    }

    GameEngine::const_iterator GameEngine::cend() const
    {
        return this->mGameObjects.cend();
    }

    GameEngine::const_iterator GameEngine::begin() const
    {
        return this->mGameObjects.begin();
    }

    GameEngine::const_iterator GameEngine::end() const
    {
        return this->mGameObjects.end();
    }


/**
 * @method Run
 * @arguments None
 * @description Gives control to the game engine. Will not return until the game window is closed or
 * all objects have been destroyed.
    */
    void GameEngine::Run() {
        
        GameContext context;
        NotificationManager notificationManager;
        context.mEngineView = this;
        context.ScreenContext = mDrawContext.get();
        context.GUIContext = mGUIContext.get();
        context.mNotificationManager = &notificationManager;
        context.currentFrame = 0;       //Frame 0 or 1 to start??
        

        while (mWindow->isOpen())  // window is open
        {
            // 0. Remove any objects that are now dead
            size_t i = 0;
            while (i < mGameObjects.size())
            {
                if (mGameObjects[i]->IsAlive())
                {
                    i++;
                }
                else
                {
                    // Replace the dead object with the last object, then shrink the vector
                    mGameObjects[i] = mGameObjects.back();
                    mGameObjects.pop_back();
                }
            }

            // 1. Activate and initialize any objects added during the last frame
            std::vector<std::shared_ptr<GameObject>> toAdd;
            toAdd.swap(mPendingGameObjects); 
            for (auto& obj : toAdd) {
                context.CurrObject = obj;
                obj->Initialize(&context);
                mGameObjects.push_back(obj);
            }

            // 2. Process events
            auto sendNotification = [&](const std::unordered_map<int, std::vector<std::string>> &table, int keyCode) {
                        auto it = table.find(keyCode);
                        if (it == table.end()) {
                            return;
                        }
                        for (const auto &message : it->second) {
                            notificationManager.Notify(message);
                        }
            };

            while (const std::optional event = mWindow->pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    mWindow->close();
                }
                else if (const auto *key = event->getIf<sf::Event::KeyPressed>()) {
                    sendNotification(mKeyDownNotifications, static_cast<int>(key->scancode));
                }
                else if (const auto *key = event->getIf<sf::Event::KeyReleased>()) {
                    sendNotification(mKeyUpNotifications, static_cast<int>(key->scancode));
                }
            }
            
            // while (const std::optional event = mWindow->pollEvent())
            // {
            //     if (event->is<sf::Event::Closed>())
            //     {
            //        mWindow->close();
            //     }
            //     else
            //     {
                    // std::string Shoot = "Shoot";
                    // std::string Left = "MoveLeft";
                    // std::string Right = "MoveRight";
                    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space))
                    // {
                    //     InstallKeyDownNotification(Key::Space, Shoot);
                    // }

                    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
                    // {
                    //     InstallKeyDownNotification(Key::A, Left);
                    // }
                    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left))
                    // {
                    //     InstallKeyDownNotification(Key::Left, Left);
                    // }

                    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
                    // {
                    //     InstallKeyDownNotification(Key::D, Right);
                    // }
                    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right))
                    // {
                    //     InstallKeyDownNotification(Key::Right, Right);
                    // }
            //     }
            // }

            // 3. Update game objects
            for (auto object: mGameObjects)
            {
                context.CurrObject = object;
                object->Update(&context);
            }
            
            // Activate objects crated during Update
            std::vector<std::shared_ptr<GameObject>> newObjects;
            newObjects.swap(mPendingGameObjects);
            for (auto &obj: newObjects) {
                context.CurrObject = obj;
                obj->Initialize(&context);
                mGameObjects.push_back(obj);
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
                    //Check if not colliding to determine if objects do collide
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
                context.CurrObject = object;
                object->LateUpdate(&context);
            }

            // 6. Get offsets and rotations
            // for (auto &object: mGameObjects)
            //{
            //
            //}

            // Clear window
            mWindow->clear();

            // 7. Render background
            for (auto &obj : mGameObjects)
            {
                auto g = std::dynamic_pointer_cast<GraphicsObject>(obj);
                if (g) {
                    context.CurrObject = obj;
                    mDrawContext->SetContextOffset(g->GetLocation());
                    mDrawContext->SetContextRotation(g->GetRotation());
                    g->RenderBackground(&context);
                }
            }

            // 8. Render foreground
            for (auto &obj : mGameObjects)
            {
                auto g = std::dynamic_pointer_cast<GraphicsObject>(obj);
                if (g) {
                    context.CurrObject = obj;
                    mDrawContext->SetContextOffset(g->GetLocation());
                    mDrawContext->SetContextRotation(g->GetRotation());
                    g->RenderForeground(&context);
                
                }
            }

            // 9. Render UI on top of everything
            for (auto &obj : mGameObjects) {
                context.CurrObject = obj;
                obj->RenderUI(&context);
            }

            // Actually render to window
            context.currentFrame++;
            mWindow->display();
        }
    }
}  // namespace CMPUT350
