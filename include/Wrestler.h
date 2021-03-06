#ifndef WRESTLER_H
#define WRESTLER_H
#include "SFML/Graphics.hpp"

#include "Collidable.h"

class Wrestler: public Collidable
{
    public:
        Wrestler();
        virtual ~Wrestler();

        void init(int hit_height, int hit_width, int x, int y);

        void reset(std::vector<Collidable*> &actors, sf::Vector2f checkpoint);

        void useGrab(Wrestler* grabee, int level_w, int level_h);
        void useThrow(Wrestler* throwee, int level_w, int level_h);
        void useDash();

        void increaseStamina();
        void depleteStamina(int loss);

        //General getters and setters:
        int getStrength();
        int getStamina();
        float getAcceleration(); //Currently based on strength/weight

        void setCharacter(std::string character_name);

        void setStats(float spd, int str, float wgt, int stm);
        bool isHuman();
        void setIsHuman(bool val);

        // used for properly alternating sprites
        void setUpState(bool val);
        void setDownState(bool val);

        bool getUpState();
        bool getDownState();

        void setStep(bool val);
        bool getStep();

        enum State{NORMAL = 0, GRABBING = 1, GRABBED = 2, THROWN = 3, DASH = 4, NOGRAB = 5};

        enum SpriteState{STAND_RIGHT = 0, STAND_LEFT = 1, STAND_UP = 2,
        STAND_DOWN = 3, RUN_RIGHT1 = 4, RUN_RIGHT2 = 5, RUN_LEFT1 = 6,
        RUN_LEFT2 = 7, RUN_UP1 = 8, RUN_UP2 = 9, RUN_DOWN1 = 10, RUN_DOWN2 = 11,
        DASH_RIGHT = 12, DASH_LEFT = 13, DASH_UP = 14, DASH_DOWN = 15};

        State getCurrentState();
        void setCurrentState(State new_state);

        SpriteState getCurSpriteState();
        void setCurSpriteState(SpriteState new_state);

        int getFrozenFrames();
        void incFrozenFrames();
        void resetFrozenFrames();

        long int getIDOfGrabbed();
        void setIDOfGrabbed(long int id);

    protected:
    private:
        //attributes
        int strength;
        int stamina;
        float acceleration;

        bool is_human;

        // used for properly switching sprite states
        bool first_up_state;
        bool first_down_state;
        bool first_step;

        State current_state;
        SpriteState sprite_state;
        int frozen_frames;

        long int id_of_grabbed;
};

#endif // WRESTLER_H
