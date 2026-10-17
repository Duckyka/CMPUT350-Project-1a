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
        
        // for (auto &obj : mGameObjects)
        // {
        //     auto g = std::dynamic_pointer_cast<Galaga>(obj);
        //     if (g)
        //     {
        //         obj
        //     }
        // }

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
            struct CollisionEntry
            {
                std::shared_ptr<CollisionObject> object;
                Rect bounds;                       // screen-space bounding box
                std::vector<Shape> screenShapes;   // filled in only when first needed
                bool shapesReady = false;
            };

            std::vector<CollisionEntry> entries;
            for (auto &obj : mGameObjects)
            {
                auto collidable = std::dynamic_pointer_cast<CollisionObject>(obj);
                if (collidable) entries.push_back({collidable, collidable->GetBounds(), {}, false});
            }
            
            // Sort by the left edge so we can stop checking once boxes are too far right
            std::sort(entries.begin(), entries.end(),
                        [](const CollisionEntry &left, const CollisionEntry &right) {
                            return left.bounds.topLeft.x < right.bounds.topLeft.x;
                        });

            // Converts an object's shapes to screen space the first time they're needed
            auto getScreenShapes = [](CollisionEntry &entry) -> const std::vector<Shape> & {
                if (!entry.shapesReady)
                {
                    Point2D location = entry.object->GetLocation();
                    float rotation = entry.object->GetRotation();
                    for (const Shape &localShape : entry.object->GetShapes())
                        entry.screenShapes.push_back(ToWorld(localShape, location, rotation));
                    entry.shapesReady = true;
                }
                return entry.screenShapes;
            };

            // j starts at i + 1 so each pair is only compared one
            for (size_t i = 0; i < entries.size(); ++i)
            {
                CollisionEntry &entryA = entries[i];
                float rightEdgeA = entryA.bounds.topLeft.x + entryA.bounds.width;
                
                for (size_t j = i + 1; j < entries.size(); ++j)
                {
                    CollisionEntry &entryB = entries[j];

                    // Sorted by left edge: if B starts past A's right edge, so does everything after B
                    if (entryB.bounds.topLeft.x > rightEdgeA)
                        break;

                    // Cheap check first: skip this pair if the bounding boxes don't touch
                    if (!Overlaps(entryA.bounds, entryB.bounds))
                        continue;

                    const std::vector<Shape> &screenShapesA = getScreenShapes(entryA);
                    const std::vector<Shape> &screenShapesB = getScreenShapes(entryB);

                    // Test every shape in A against every shape in B.
                    // If lines are involved, keep the hit closest to the start of the line.
                    bool isColliding = false;
                    bool hasCollisionPoint = false;
                    float closestFraction = 2; // anything above 1 means "no point yet"
                    Point2D closestPoint;

                    for (const Shape &shapeA : screenShapesA) {
                        for (const Shape &shapeB : screenShapesB) {
                            
                            bool hitHasPoint;
                            Point2D hitPoint;
                            float hitFraction;
                            if (!ShapesCollide(shapeA, shapeB, hitHasPoint, hitPoint, hitFraction)) {
                                continue;
                            }
                            isColliding = true;
                            if (hitHasPoint && hitFraction < closestFraction) {
                                closestFraction = hitFraction;
                                closestPoint = hitPoint;
                                hasCollisionPoint = true;
                            }
                        }
                    }

                    if (!isColliding) {
                        continue;
                    }
                    
                    auto &objectA = entryA.object;
                    auto &objectB = entryB.object;
                    if (hasCollisionPoint)
                    {   
                        // Give each object the collision point in its own local space
                        mDrawContext->SetContextOffset(objectA->GetLocation());
                        mDrawContext->SetContextRotation(objectA->GetRotation());
                        Point2D localPointA = mDrawContext->ReverseTransform(closestPoint);

                        mDrawContext->SetContextOffset(objectB->GetLocation());
                        mDrawContext->SetContextRotation(objectB->GetRotation());
                        Point2D localPointB = mDrawContext->ReverseTransform(closestPoint);

                        // Bounding boxes touch. For now, behave the same as before.
                        // Later this becomes the shape-by-shape check with ShapesCollide().
                        objectA->CollisionEnter(objectB, localPointA);
                        objectB->CollisionEnter(objectA, localPointB);
                    }
                    else {
                        objectA->CollisionEnter(objectB);
                        objectB->CollisionEnter(objectA);
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
