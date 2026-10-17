#include "GraphicsObject.h"

namespace CMPUT350 {

// Draw behind everything else. Does nothing by default.
void GraphicsObject::RenderBackground(GameContext *context) {}

// Draw in front of the background. Does nothing by default.
void GraphicsObject::RenderForeground(GameContext *context) {}

}  // namespace CMPUT350
