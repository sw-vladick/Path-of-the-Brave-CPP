#include "HotBar.h"

using namespace std;

HotBar::HotBar(ResourceManager& res)
{
	rm = &res;
	selectedSlot = 0;
	float startX = 1550.f;
	float startY = 50.f;
	float offset = 80.f;

	
	for (int i = 0; i < 4; i++)
	{
		slots[i] = "";

		UI[i].setSize(sf::Vector2f(60.f, 60.f));
		UI[i].setFillColor(sf::Color(100, 100, 100, 200));

		UI[i].setOutlineThickness(3.f);
		UI[i].setOutlineColor(sf::Color::White);

		UI[i].setPosition(startX + (i * offset), startY);
	}

	UI[0].setOutlineColor(sf::Color::Red);
}

void HotBar::drawHUD(sf::RenderWindow& window, std::map<std::string, InventoryItem>& playerInventory)
{
	sf::Text text;
	text.setFont(rm->font);
	text.setOutlineThickness(1.f);
	text.setOutlineColor(sf::Color::Black);

	for (int i = 0; i < 4; i++)
	{
		window.draw(UI[i]);

		text.setCharacterSize(14);
		text.setFillColor(sf::Color(200, 200, 200));
		text.setString(std::to_string(i + 1));
		text.setPosition(UI[i].getPosition().x + 5.f, UI[i].getPosition().y + 40.f);
		window.draw(text);

		if (slots[i] != "") {
			string itemName = slots[i];
			iconSprite.setTexture(rm->textures[itemName]);
			iconSprite.setPosition(UI[i].getPosition().x + 13.f, UI[i].getPosition().y + 13.f);
			window.draw(iconSprite);

			if (playerInventory.count(itemName) > 0) {
				int count = playerInventory[itemName].count;
				int lvl = playerInventory[itemName].level;
				bool isUpgradable = playerInventory[itemName].isUpgradable;

				if (isUpgradable && lvl > 0) {
					text.setCharacterSize(14);
					text.setFillColor(sf::Color::Cyan);
					text.setString("Lv." + std::to_string(lvl));
					text.setPosition(UI[i].getPosition().x + 28.f, UI[i].getPosition().y + 2.f);
					window.draw(text);
				}
				else if (!isUpgradable && count > 1) {
					text.setCharacterSize(16);
					text.setFillColor(sf::Color::Yellow);
					text.setString(std::to_string(count));
					text.setPosition(UI[i].getPosition().x + 40.f, UI[i].getPosition().y + 2.f);
					window.draw(text);
				}
			}
			else {
				ClearSlot(i);
			}
		}
	}
}

int HotBar::GetSlotIndex(sf::Vector2f mousePos)
{
	for (int i = 0; i < 4; i++)
	{
		if (UI[i].getGlobalBounds().contains(mousePos)) {
			
			return i;
		}
	}
	return -1;
}

bool HotBar::AddItem(std::string itemName)
{
	for (int i = 0; i < 4; i++) {
		if (slots[i] == itemName) {
			return true;
		}
	}

	for (int i = 0; i < 4; i++) {
		if (slots[i] == "") {
			slots[i] = itemName;
			return true;
		}
	}

	return false;
}

string HotBar::UseItem()
{
	return slots[selectedSlot];
}

void HotBar::update()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
		UI[selectedSlot].setOutlineColor(sf::Color::White);
		selectedSlot = 0;
		UI[selectedSlot].setOutlineColor(sf::Color::Red);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
		UI[selectedSlot].setOutlineColor(sf::Color::White);
		selectedSlot = 1;
		UI[selectedSlot].setOutlineColor(sf::Color::Red);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
		UI[selectedSlot].setOutlineColor(sf::Color::White);
		selectedSlot = 2;
		UI[selectedSlot].setOutlineColor(sf::Color::Red);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
		UI[selectedSlot].setOutlineColor(sf::Color::White);
		selectedSlot = 3;
		UI[selectedSlot].setOutlineColor(sf::Color::Red);
	}
}

string HotBar::GetCurrentItem(int index)
{
	return slots[index];
}

void HotBar::ClearSlot(int index) { slots[index] = ""; }
void HotBar::SetSlot(int index, std::string name) { slots[index] = name; }