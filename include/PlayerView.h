#ifndef PLAYERVIEW_H
#define PLAYERVIEW_H
#include "SFML/Graphics.hpp"
#include "Collidable.h"
#include "Profile.h"
#include "Wrestler.h"
#include "Terrain.h"


class PlayerView
{
    public:
        PlayerView();
        virtual ~PlayerView();

        void init();
        void drawLevel(sf::RenderWindow& App, Terrain& level, Terrain& layer);
        void drawActors(sf::RenderWindow& App, std::vector<Collidable*> actors);
        void drawHUD(sf::RenderWindow& App, Profile& profile);
        void drawStaminaBar(sf::RenderWindow& App, Collidable* player);

    protected:
    private:
        void setSprite(Collidable* actor);
        sf::Texture sprite_sheet;
        sf::Sprite sprite;
        sf::Font font;
        sf::RectangleShape stamBar;
        sf::RectangleShape stamBack;
        sf::RectangleShape mid;
};

#endif // PLAYERVIEW_H
