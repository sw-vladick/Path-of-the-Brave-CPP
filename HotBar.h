#pragma once
#include <string>
#include <map>	
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"

struct InventoryItem {
	int count;          
	int level;         
	bool isUpgradable; 
};

class HotBar
{
	int selectedSlot;
	std::string slots[4];
	sf::RectangleShape UI[4];
	ResourceManager* rm;
	sf::Sprite iconSprite;
	sf::Text numberSlot;
public:
	HotBar(ResourceManager& res);
	void drawHUD(sf::RenderWindow& window, std::map<std::string, InventoryItem>& playerInventory);
	int GetSlotIndex(sf::Vector2f mousePos);
	bool AddItem(std::string itemName);
	std::string UseItem();
	void update();
	std::string GetCurrentItem(int index);
	void ClearSlot(int index);
	void SetSlot(int index, std::string name);
};