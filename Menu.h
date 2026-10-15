#pragma once
#include "SFML/Graphics.hpp"
#include "DatabaseManager.h"

class Menu
{
	int indexpos;
	sf::Font font;
	sf::Text menu[3];
	bool isSaveMenuOpen;
	sf::RectangleShape saveSlots[3];
	sf::Text saveSlotTexts[3];
	bool isPaused;
	sf::RectangleShape deleteBtns[3];
	sf::Text deleteTexts[3];
	std::vector<SaveSlotInfo> currentSlots;
	bool isWaitingForKey;
	bool isSettingsOpen;
	sf::Text settingsMenu[3];
	int masterVolume;
	sf::Keyboard::Key potionKey;
public:
	Menu(float x, float y, sf::Font& f);
	void draw(sf::RenderWindow& window);
	void MoveUp();
	void MoveDown();
	int GetIndex();
	void UpdateMouse(sf::Vector2f m);
	bool isMouseOverSelectedItem(sf::Vector2f mousePos);
	void UpdateSaveSlots(std::vector<SaveSlotInfo>& slotsInfo);
	bool IsSaveMenuOpen();
	void SetSaveMenuOpen(bool state);
	int GetClickedSlot(sf::Vector2f mousePos);
	bool GetPaused();
	int GetClickedDeleteButton(sf::Vector2f mousePos);
	void SetPaused(bool pause);
	bool IsSettingsOpen();
	void SetSettingsOpen(bool state);
	void UpdateSettingsText();
	int GetVolume();
	void SetVolume(int vol);
	sf::Keyboard::Key GetPotionKey();
	void SetPotionKey(sf::Keyboard::Key key);
	bool IsWaitingForKey();
	void SetWaitingForKey(bool state);
	int GetClickedSettingsButton(sf::Vector2f mousePos);
	std::string KeyToString(sf::Keyboard::Key key);
};

