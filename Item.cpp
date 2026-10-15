#include "Item.h"
#include <cstdlib>

Item::Item()
{
	isActive = false;
	gravity = Config::GRAVITY;
	onGround = false;
}

void Item::Spawn(std::string name, sf::Texture& texture, float startX, float startY)
{
	itemName = name;

	ItemSprite.setTexture(texture);

	

	sf::Vector2u size = texture.getSize();
	if (name == "Coin") {
		ItemSprite.setScale(0.1f, 0.1f);
	}
	else {
		ItemSprite.setScale(1.5f, 1.5f);
	}
	ItemSprite.setOrigin(size.x / 2.0f, size.y / 2.0f);
	ItemSprite.setPosition(startX, startY);


	dy = -800.f; 

	if (rand() % 2 == 0) {
		dx = -300.f;
	}
	else {
		dx = 300.f;
	}

	isActive = true;
	onGround = false;
}

void Item::Update(float dt)
{
	if (!isActive) return;

	if (!onGround) {
		dy += gravity * dt;
		ItemSprite.move(dx * dt, dy * dt);

		if (ItemSprite.getPosition().y >= Config::GROUND_Y) {
			ItemSprite.setPosition(ItemSprite.getPosition().x, 840);
			dy = 0;
			dx = 0;
			onGround = true;
		}
	}
}

void Item::Draw(sf::RenderWindow& window)
{
	if (isActive) {
		window.draw(ItemSprite);
	}
}

bool Item::IsActive() { return isActive; }
void Item::PickUp() { isActive = false; }
sf::FloatRect Item::getBounds() { return ItemSprite.getGlobalBounds(); }
std::string Item::getName() { return itemName; }