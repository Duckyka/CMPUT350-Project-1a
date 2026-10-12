#ifndef GALAGA_H
#define GALAGA_H

namespace CMPUT350 {

class Galaga
{
public:
    enum States{Insert, Startup, Waves, Play, End};

    void Initialize(States currentState);

    States CheckState(States currentState);
    //setUpNotifications(
private:
    bool nextState;
    States currentState;
};

} //Namespace CMUT350

#endif //GALAGA_H