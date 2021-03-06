#include "Profile.h"

Profile::Profile()
{
    //ctor
}

void Profile::setCharacter(std::string char_name)
{
    character = char_name;
}

std::string Profile::getCharacter()
{
    return character;
}

void Profile::setLevel(std::string level_name)
{
    level = level_name;
}

std::string Profile::getLevel()
{
    return level;
}

void Profile::setLives(int remaining)
{
    lives = remaining;
}

int Profile::livesRemaining()
{
    return lives;
}

void Profile::setCheckpoint(unsigned int checkpoint_num)
{
    cur_checkpoint_num = checkpoint_num;
}

unsigned int Profile::getCheckpoint()
{
    return cur_checkpoint_num;
}

Profile::~Profile()
{
    //dtor
}
