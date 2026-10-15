#pragma once
#include "Config.h"
#include <SFML/Graphics.hpp>

class Chest
{
	sf::Texture ChestTexture;
	sf::Sprite ChestSprite;
	int ChestX;
	bool StayChest;
	int Item;
	
public:
	std::string entityId;
	Chest(sf::Texture& tex);
	void SetPosition(float x); 
	void SetChest();
	void DrawChest(sf::RenderWindow& window);
	void ChestOpen();
	bool MouseOnChest(sf::Vector2f ChestMouse);
	int GetPositionChest();
	int DropItem();
};

