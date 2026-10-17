#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>

namespace CMPUT350 {

class GameContext;

/*
Base class for everything in the game. The engine calls these functions
every frame. Key presses arrive through ReceiveNotification.
*/
class GameObject {
public:
    virtual ~GameObject() = default;
    virtual void Initialize(GameContext *context);
    virtual void Update(GameContext *context);
    virtual void LateUpdate(GameContext *context);
    virtual void RenderUI(GameContext *context);
    virtual bool IsAlive() const;
    virtual void Kill();
    virtual void ReceiveNotification(const std::string& key);
};

}  // namespace CMPUT350

#endif  // GAMEOBJECT_H
