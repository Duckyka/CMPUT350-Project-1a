#include "GameObject.h"
#include <string>

namespace CMPUT350 {

// Called once when the object is added to the engine. Does nothing by default.
void GameObject::Initialize(GameContext *context) {}

// Called every frame before collisions. Does nothing by default.
void GameObject::Update(GameContext *context) {}

// Called every frame after collisions. Does nothing by default.
void GameObject::LateUpdate(GameContext *context) {}

// Called every frame to draw on top of everything. Does nothing by default.
void GameObject::RenderUI(GameContext *context) {}

// Output: true while the object should stay in the engine. Always true by default.
bool GameObject::IsAlive() const { return true; }

// Tells the object to return false from IsAlive. Does nothing by default.
void GameObject::Kill() {}

// Called by the notification manager when a message the object registered for is sent.
void GameObject::ReceiveNotification(const std::string& key) {}

}  // namespace CMPUT350
