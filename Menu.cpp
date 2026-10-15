#include "Menu.h"
#include "SFML/Graphics.hpp"


Menu::Menu(float x, float y, sf::Font& f)
{

	isPaused = false;
	font = f;

	std::string menuNames[3] = { "Play", "Settings", "Exit" };
	for (int i = 0; i < 3; i++) {
		menu[i].setFont(font);
		menu[i].setString(menuNames[i]);
		menu[i].setCharacterSize(70);
		menu[i].setFillColor(i == 0 ? sf::Color::White : sf::Color::Black);

		sf::FloatRect textRect = menu[i].getLocalBounds();
		menu[i].setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
		menu[i].setPosition(sf::Vector2f(x / 2.0f, (y / 2.0f - 100.f) + (i * 100.f)));
	}
	indexpos = 0;

	isSettingsOpen = false;
	masterVolume = 100;
	potionKey = sf::Keyboard::H;
	isWaitingForKey = false;

	for (int i = 0; i < 3; i++) {
		settingsMenu[i].setFont(font);
		settingsMenu[i].setCharacterSize(50);
		settingsMenu[i].setFillColor(sf::Color::White);
		settingsMenu[i].setPosition(x / 2.0f - 200.f, y / 2.0f - 100.f + (i * 100.f));
	}
	UpdateSettingsText(); 
	
	isSaveMenuOpen = false;

	float slotWidth = 800.f;
	float slotHeight = 100.f;

	
	float startX = (x / 2.0f) - (slotWidth / 2.0f);
	float startY = 200.f;

	for (int i = 0; i < 3; i++) {
		saveSlots[i].setSize(sf::Vector2f(slotWidth, slotHeight));
		saveSlots[i].setPosition(startX, startY + (i * 150.f));

		saveSlots[i].setFillColor(sf::Color(40, 40, 40, 230));
		saveSlots[i].setOutlineThickness(3.f);
		saveSlots[i].setOutlineColor(sf::Color::White);

		saveSlotTexts[i].setFont(font);
		saveSlotTexts[i].setCharacterSize(30);
		saveSlotTexts[i].setFillColor(sf::Color::White);
		
		saveSlotTexts[i].setPosition(startX + 20.f, startY + (i * 150.f) + 30.f);
		saveSlotTexts[i].setString("Empty Slot");

		deleteBtns[i].setSize(sf::Vector2f(60.f, 60.f));
		deleteBtns[i].setPosition(startX + slotWidth + 20.f, startY + (i * 150.f) + 20.f);
		deleteBtns[i].setFillColor(sf::Color(180, 0, 0));
		deleteBtns[i].setOutlineThickness(2.f);
		deleteBtns[i].setOutlineColor(sf::Color::White);

		deleteTexts[i].setFont(font);
		deleteTexts[i].setCharacterSize(40);
		deleteTexts[i].setFillColor(sf::Color::White);
		deleteTexts[i].setString("X");
		deleteTexts[i].setPosition(deleteBtns[i].getPosition().x + 15.f, deleteBtns[i].getPosition().y + 2.f);
	}
}

void Menu::MoveUp()
{
	if (indexpos - 1 >= 0)
	{
		menu[indexpos].setFillColor(sf::Color::Black);
		indexpos--;
		menu[indexpos].setFillColor(sf::Color::White);
	}
}

void Menu::MoveDown()
{

	if (indexpos + 1 < 3)
	{
		menu[indexpos].setFillColor(sf::Color::Black);
		indexpos++;
		menu[indexpos].setFillColor(sf::Color::White);
	}
}

int Menu::GetIndex()
{
	return indexpos;
}

void Menu::UpdateMouse(sf::Vector2f m)
{
	for (int i = 0; i < 3; i++)
	{
		if (menu[i].getGlobalBounds().contains(m)) {
			menu[indexpos].setFillColor(sf::Color::Black);
			indexpos = i;
			menu[indexpos].setFillColor(sf::Color::White);
		}
	}
}

bool Menu::isMouseOverSelectedItem(sf::Vector2f mousePos)
{

	return menu[indexpos].getGlobalBounds().contains(mousePos);
}

void Menu::draw(sf::RenderWindow& window) {
	if (isSaveMenuOpen) {
		for (int i = 0; i < 3; i++) {
			window.draw(saveSlots[i]);
			window.draw(saveSlotTexts[i]);
			if (!currentSlots.empty() && !currentSlots[i].isEmpty) {
				window.draw(deleteBtns[i]);
				window.draw(deleteTexts[i]);
			}
		}
	}
	else if (isSettingsOpen) {
		for (int i = 0; i < 3; i++) {
			window.draw(settingsMenu[i]);
		}
	}
	else {
		for (int i = 0; i < 3; i++) {
			window.draw(menu[i]);
		}
	}
}


bool Menu::IsSaveMenuOpen() {
	return isSaveMenuOpen;
}

void Menu::SetSaveMenuOpen(bool state) {
	isSaveMenuOpen = state;
}

void Menu::UpdateSaveSlots(std::vector<SaveSlotInfo>& slotsInfo) {
	currentSlots = slotsInfo;
	for (int i = 0; i < 3; i++) {
		if (slotsInfo[i].isEmpty) {
			saveSlotTexts[i].setString("Slot " + std::to_string(slotsInfo[i].slotNumber) + ": Empty (New Game)");
		}
		else {
			saveSlotTexts[i].setString("Slot " + std::to_string(slotsInfo[i].slotNumber) +
				" | Level: " + std::to_string(slotsInfo[i].level) +
				" | Saved: " + slotsInfo[i].saveDate);
		}
	}
}

int Menu::GetClickedSlot(sf::Vector2f mousePos) {
	if (!isSaveMenuOpen) return 0;

	for (int i = 0; i < 3; i++) {
		
		if (saveSlots[i].getGlobalBounds().contains(mousePos)) {
			return i + 1; 
		}
	}
	return 0;
}

bool Menu::GetPaused()
{
	return isPaused;
}

void Menu::SetPaused(bool pause)
{
	isPaused = pause;

	if (isPaused) {
		menu[0].setString("Resume");
	}
	else {
		menu[0].setString("Play");
	}

	
	sf::FloatRect textRect0 = menu[0].getLocalBounds();
	menu[0].setOrigin(textRect0.left + textRect0.width / 2.0f, textRect0.top + textRect0.height / 2.0f);
}

int Menu::GetClickedDeleteButton(sf::Vector2f mousePos) {
	if (!isSaveMenuOpen || currentSlots.empty()) return 0;

	for (int i = 0; i < 3; i++) {
		if (!currentSlots[i].isEmpty && deleteBtns[i].getGlobalBounds().contains(mousePos)) {
			return i + 1; 
		}
	}
	return 0;
}

bool Menu::IsSettingsOpen() { return isSettingsOpen; }
void Menu::SetSettingsOpen(bool state) { isSettingsOpen = state; }

void Menu::UpdateSettingsText() {
	settingsMenu[0].setString("Volume: " + std::to_string(masterVolume) + "%  [<-] [->]");

	if (isWaitingForKey) {
		settingsMenu[1].setString("Potion Key: [ PRESS ANY KEY ]");
		settingsMenu[1].setFillColor(sf::Color::Red);
	}
	else {
		settingsMenu[1].setString("Potion Key: [" + KeyToString(potionKey) + "]");
		settingsMenu[1].setFillColor(sf::Color::White);
	}

	settingsMenu[2].setString("Back to Menu");
}

int Menu::GetVolume() { return masterVolume; }
void Menu::SetVolume(int vol) { masterVolume = vol; UpdateSettingsText(); }
sf::Keyboard::Key Menu::GetPotionKey() { return potionKey; }
void Menu::SetPotionKey(sf::Keyboard::Key key) { potionKey = key; UpdateSettingsText(); }
bool Menu::IsWaitingForKey() { return isWaitingForKey; }
void Menu::SetWaitingForKey(bool state) { isWaitingForKey = state; UpdateSettingsText(); }

int Menu::GetClickedSettingsButton(sf::Vector2f mousePos) {
	if (!isSettingsOpen) return -1;
	for (int i = 0; i < 3; i++) {
		if (settingsMenu[i].getGlobalBounds().contains(mousePos)) return i;
	}
	return -1;
}

std::string Menu::KeyToString(sf::Keyboard::Key key) {
	if (key >= sf::Keyboard::A && key <= sf::Keyboard::Z) {
		return std::string(1, 'A' + (key - sf::Keyboard::A));
	}
	if (key >= sf::Keyboard::Num0 && key <= sf::Keyboard::Num9) {
		return std::string(1, '0' + (key - sf::Keyboard::Num0));
	}
	return "Key " + std::to_string(key);
}