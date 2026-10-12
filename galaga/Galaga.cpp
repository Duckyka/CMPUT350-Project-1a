#include "Galaga.h"
#include "Player.h"
#include "Enemy.h"

//Enum of all game states
enum States{Insert, Startup, Waves, Play, End};

//Initialize function, sets up Galaga object, starting with Insert state. 
void Initialize(States currentState)
{
    currentState = States::Insert;
}

//CheckState function, transitions between states. 
States CheckState(States currentState, bool nextState)
{
    if(currentState == States::Insert && nextState)
    {
        nextState = false;
    }
    else if(currentState == States::Startup && nextState)
    {
        nextState = false;
    }
    else if(currentState == States::Waves && nextState)
    {
        nextState = false;
    }
    else if(currentState == States::Play && nextState)
    {
        nextState = false;
    }
    else if(currentState == States::End && nextState)
    {
        nextState = false;
    }
}



