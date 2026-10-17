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

namespace CMPUT350 {

/*
Create the window, load the font, and make the two draw contexts
(one for game objects, one for the GUI).
Input:  width, height - size of the game area in pixels
        name - the window title
*/
GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name)
{
    // The window is shown smaller so it fits on screen, but the game still
    // works in width x height coordinates through the view
    constexpr float kWindowScale = 0.84f;
    unsigned int windowWidth = static_cast<unsigned int>(width * kWindowScale);
    unsigned int windowHeight = static_cast<unsigned int>(height * kWindowScale);
    mWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode({windowWidth, windowHeight}), name);
    mWindow->setFramerateLimit(30);
    mWindow->setView(sf::View(sf::FloatRect({0.0f, 0.0f}, {static_cast<float>(width), static_cast<float>(height)})));

    // Load the font from embedded data
    mFont = std::make_shared<sf::Font>();
    if (!mFont->openFromMemory(_font, sizeof(_font))) {
        std::cerr << "Failed to load font\n";
    }

    mDrawContext = std::make_shared<DrawContext>(mWindow, mFont);
    mGUIContext = std::make_shared<DrawContext>(mWindow, mFont);

    // Only send one key down event per press, not repeats while held
    mWindow->setKeyRepeatEnabled(false);
}

/*
Close the window when the engine is destroyed.
*/
GameEngine::~GameEngine()
{
    mWindow->close();
}

/*
Add an object to the engine. It is initialized and starts updating on the
next pass of the game loop, so objects can safely be added during a frame.
*/
void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
    mPendingGameObjects.push_back(gameObject);
}

/*
When the key with this scancode is pressed, send this notification
to the notification manager. A key can have more than one notification.
*/
void GameEngine::InstallKeyDownNotification(int keyCode, const std::string &notification)
{
    mKeyDownNotifications[keyCode].push_back(notification);
}

/*
When the key with this scancode is released, send this notification
to the notification manager. A key can have more than one notification.
*/
void GameEngine::InstallKeyUpNotification(int keyCode, const std::string &notification)
{
    mKeyUpNotifications[keyCode].push_back(notification);
}

/*
Output: const iterator to the first object in the engine.
*/
GameEngine::const_iterator GameEngine::cbegin() const
{
    return mGameObjects.cbegin();
}

/*
Output: const iterator to one past the last object in the engine.
*/
GameEngine::const_iterator GameEngine::cend() const
{
    return mGameObjects.cend();
}

/*
Output: const iterator to the first object, so the engine works in range-based for loops.
*/
GameEngine::const_iterator GameEngine::begin() const
{
    return mGameObjects.begin();
}

/*
Output: const iterator to one past the last object, so the engine works in range-based for loops.
*/
GameEngine::const_iterator GameEngine::end() const
{
    return mGameObjects.end();
}

/*
Run the game loop until the window is closed.
Each frame: remove dead objects, add new ones, handle keys, update,
check collisions, late update, then draw.
*/
void GameEngine::Run()
{
    GameContext context;
    NotificationManager notificationManager;
    context.mEngineView = this;
    context.ScreenContext = mDrawContext.get();
    context.GUIContext = mGUIContext.get();
    context.mNotificationManager = &notificationManager;
    context.currentFrame = 0;

    // Initialize any new objects and move them into the main list
    auto addPendingObjects = [&]() {
        std::vector<std::shared_ptr<GameObject>> toAdd;
        toAdd.swap(mPendingGameObjects);
        for (auto &obj : toAdd) {
            context.CurrObject = obj;
            obj->Initialize(&context);
            mGameObjects.push_back(obj);
        }
    };

    // Send every notification registered for this key
    auto sendNotifications = [&](const std::unordered_map<int, std::vector<std::string>> &table, int keyCode) {
        auto it = table.find(keyCode);
        if (it == table.end()) {
            return;
        }
        for (const auto &message : it->second) {
            notificationManager.Notify(message);
        }
    };

    while (mWindow->isOpen())
    {
        // 1. Remove dead objects
        size_t i = 0;
        while (i < mGameObjects.size())
        {
            if (mGameObjects[i]->IsAlive()) {
                i++;
            }
            else {
                // Replace the dead object with the last object, then shrink the vector
                mGameObjects[i] = mGameObjects.back();
                mGameObjects.pop_back();
            }
        }

        // 2. Add objects created during the last frame
        addPendingObjects();

        // 3. Turn key presses and releases into notifications
        while (const std::optional event = mWindow->pollEvent())
        {
            if (event->is<sf::Event::Closed>()) {
                mWindow->close();
            }
            else if (const auto *key = event->getIf<sf::Event::KeyPressed>()) {
                sendNotifications(mKeyDownNotifications, static_cast<int>(key->scancode));
            }
            else if (const auto *key = event->getIf<sf::Event::KeyReleased>()) {
                sendNotifications(mKeyUpNotifications, static_cast<int>(key->scancode));
            }
        }

        // 4. Update objects
        for (auto object : mGameObjects)
        {
            context.CurrObject = object;
            object->Update(&context);
        }

        // Add objects created during Update so they can collide this frame
        addPendingObjects();

        // 5. Collisions
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
            if (collidable) {
                entries.push_back({collidable, collidable->GetBounds(), {}, false});
            }
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
                for (const Shape &localShape : entry.object->GetShapes()) {
                    entry.screenShapes.push_back(ToWorld(localShape, location, rotation));
                }
                entry.shapesReady = true;
            }
            return entry.screenShapes;
        };

        // j starts at i + 1 so each pair is only compared once
        for (size_t i = 0; i < entries.size(); ++i)
        {
            CollisionEntry &entryA = entries[i];
            float rightEdgeA = entryA.bounds.topLeft.x + entryA.bounds.width;

            for (size_t j = i + 1; j < entries.size(); ++j)
            {
                CollisionEntry &entryB = entries[j];

                // Sorted by left edge: if B starts past A's right edge, so does everything after B
                if (entryB.bounds.topLeft.x > rightEdgeA) {
                    break;
                }

                // Cheap check first: skip this pair if the bounding boxes don't touch
                if (!Overlaps(entryA.bounds, entryB.bounds)) {
                    continue;
                }

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

                    objectA->CollisionEnter(objectB, localPointA);
                    objectB->CollisionEnter(objectA, localPointB);
                }
                else
                {
                    objectA->CollisionEnter(objectB);
                    objectB->CollisionEnter(objectA);
                }
            }
        }

        // 6. Late updates
        for (auto &object : mGameObjects)
        {
            context.CurrObject = object;
            object->LateUpdate(&context);
        }

        mWindow->clear();

        // 7. Draw backgrounds. Each object's location and rotation are given to
        // the draw context so objects can draw in their own local space.
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

        // 8. Draw foregrounds
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

        // 9. Draw the GUI on top of everything (no offset or rotation)
        for (auto &obj : mGameObjects)
        {
            context.CurrObject = obj;
            obj->RenderUI(&context);
        }

        context.currentFrame++;
        mWindow->display();
    }
}

}  // namespace CMPUT350
