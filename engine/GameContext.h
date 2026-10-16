#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include "DrawContext.h"
#include "EngineView.h"
#include "GameObject.h"
#include "NotificationManager.h"

namespace CMPUT350 {

/*
the GameContext class takes raw pointers of the engine, the screenContext, and notificationManager,
which allows this object to be passed through objects
engineView -> information about the engine
screenContext -> information about the screen used for drawing
notificationManager -> used to keep track of all the notifications
The pointers are owned by the engine and should not be shared.
*/
class GameContext {
public:
    EngineView *mEngineView;
    DrawContext *ScreenContext;
    DrawContext *GUIContext;
	std::weak_ptr<GameObject> CurrObject;
	NotificationManager *mNotificationManager;
	int currentFrame;
};

}  // namespace CMPUT350

#endif  // GAMECONTEXT_H
