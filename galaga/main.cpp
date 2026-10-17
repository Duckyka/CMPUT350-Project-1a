#include <memory>
#include "GameEngine.h"
#include "Galaga.h"

/*
Start the game. The Galaga object creates and manages every other object.
*/
int main()
{
    CMPUT350::GameEngine engine(768, 1024, "Galaga");
    auto game = std::make_shared<Galaga>(768, 1024);
    engine.AddGameObject(game);
    engine.Run();

    return 0;
}
