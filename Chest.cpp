#include "Chest.h"
#include <cstdlib>
#include <ctime>


Chest::Chest(sf::Texture& tex)
{
	ChestTexture = tex;
	ChestSprite.setTexture(ChestTexture);
	sf::Vector2u sizeChest = ChestTexture.getSize();
	ChestSprite.setOrigin(sizeChest.x / 2, sizeChest.y / 2);
	ChestSprite.setScale(3.f, 3.f);

	if (rand() % 2 == 0) ChestX = 50 + rand() % (1800 - 50 + 1);
	else ChestX = 2200 + rand() % (3950 - 2200 + 1);

	ChestSprite.setPosition(ChestX, Config::CHEST_SPAWN_Y);
	StayChest = true;
	Item = 0;
}

void Chest::SetPosition(float x)
{
	ChestX = x;
	ChestSprite.setPosition(ChestX, Config::CHEST_SPAWN_Y);

	StayChest = true;
	Item = 0;
}

void Chest::SetChest()
{
	
		if (rand() % 2 == 0) {
			ChestX = 50 + rand() % (1800 - 50 + 1);
		}
		else {
			ChestX = 2200 + rand() % (3950 - 2200 + 1);
		}
		ChestSprite.setPosition(ChestX, Config::CHEST_SPAWN_Y);
		StayChest = true;
}
void Chest::ChestOpen()
{
	StayChest = false;
	Item = rand() % 3;
}
bool Chest::MouseOnChest(sf::Vector2f ChestMouse)
{
	return ChestSprite.getGlobalBounds().contains(ChestMouse);
}
int Chest::GetPositionChest()
{
	return ChestX;
}
int Chest::DropItem()
{
	return Item;
}
void Chest::DrawChest(sf::RenderWindow& window)
{
	if (StayChest == true) {
		window.draw(ChestSprite);
	}
}


