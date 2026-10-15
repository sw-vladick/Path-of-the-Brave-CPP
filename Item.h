#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "Config.h"

class Item
{
	sf::Sprite ItemSprite;
	std::string itemName;
	bool isActive;
	float dx;
	float dy;
	float gravity;
	bool onGround;

public:
	Item();
	void Spawn(std::string name, sf::Texture& texture, float startX, float startY);
	void Update(float dt);
	void Draw(sf::RenderWindow& window);
	bool IsActive();
	void PickUp();
	sf::FloatRect getBounds();
	std::string getName();
};