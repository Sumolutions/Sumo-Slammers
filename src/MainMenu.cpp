#include "MainMenu.h"
#include <iostream>
#include "SFML/Graphics.hpp"

MainMenu::MainMenu()
{

}

void MainMenu::init(float width, float height)
{
	selected = 0;

	font.loadFromFile("resources/arial.ttf");

	text[0].setFont(font);
	text[0].setColor(sf::Color::White);
	text[0].setString("Play");
	text[0].setPosition(sf::Vector2f(width / 2, height / 2 ));

	text[1].setFont(font);
	text[1].setColor(sf::Color::White);
	text[1].setString("Exit");
	text[1].setPosition(sf::Vector2f(width / 2, height / 1.5));

	text[2].setFont(font);
	text[2].setColor(sf::Color::White);
	text[2].setString("~ ~ Sumo Slammers! ~ ~");
	text[2].setPosition(sf::Vector2f(width / 2, height / 4));

	highlight.setSize(sf::Vector2f(90, 40));
  	highlight.setFillColor(sf::Color(150, 0, 35));
}

void MainMenu::draw(sf::RenderWindow &window)
{
	window.clear(sf::Color::Black);

	if (selected == 0)
    {
    	highlight.setPosition(380, 300);
    } else {
    	highlight.setPosition(380, 400);
    }
	window.draw(highlight);

	window.draw(text[0]);
	window.draw(text[1]);
	window.draw(text[2]);
}

bool MainMenu::navigate(sf::RenderWindow &window, sf::Event &event)
{
	if (event.key.code == sf::Keyboard::Up)
	    {
	        moveUp();
	    }

    else if (event.key.code == sf::Keyboard::Down)
	    {
	        moveDown();
	    }

	else if (event.key.code == sf::Keyboard::Return)
	    {
	        if (selected == 0)
	        {
	        	return false;
	        } else {
	        	window.close();
	        }
	    }
    return true;
}

void MainMenu::moveUp()
{
	//if (selected == 0)
	//{
	//	selected = 1;

	//} else {
		selected = 0;
//	}
}

void MainMenu::moveDown()
{
//	if (selected == 0)
//	{
		selected = 1;
//	} else {
	//	selected = 0;
	//}
}

MainMenu::~MainMenu()
{

}
