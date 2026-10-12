#ifndef GALAGA_H
#define GALAGA_H

namespace CMPUT350 {

    class Galaga
    {
    public:
        enum States{Insert, Startup, Waves, Play, End};

        void Initialize(States currentState);
        void setUpNotifications();

        States CheckState(States currentState, bool nextState);

        using gStates = CMPUT350::Galaga::States;
    private:
        bool nextState;
        States currentState;
    };

} //Namespace CMUT350

#endif //GALAGA_H