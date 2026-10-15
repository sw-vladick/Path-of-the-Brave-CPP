#pragma once
#include <SFML/Graphics.hpp>
#include <map> 
#include <string>
#include <vector>
#include "ResourceManager.h"
#include "HotBar.h"

enum PlayerState { IDLE, RUN, JUMP, FRUN };



class Player
{
	sf::Event event;
	PlayerState currentState;
	sf::Sprite sprite;
	ResourceManager* rm;
	HotBar quickBar;
	sf::Sprite iconSprite;
	bool showInventory;
	sf::RectangleShape slotRect;
	float dy;
	float run;
	bool OnGround;
	sf::IntRect currentFrame;
	float animationTimer;
	float animationSpeed;
	int frameCount;
	int currentFrameIndex;
	bool IsMoving;
	int frameWidth;
	int frameHeight;
	std::map<std::string, InventoryItem> inventory;
	int currentHealth;
	int maxHealth;
	sf::RectangleShape hpBarBack;
	sf::RectangleShape hpBarFront;
	sf::Text inventoryText;
	sf::RectangleShape inventoryBox;
	bool isDragging;
	std::string draggedItem;
	sf::Sprite draggedSprite;
	int checkItem;
	int coins;
	float invincibilityTimer;
	int knockbackDirection;
	float potionCooldownTimer;
	bool isDead;
	sf::RectangleShape deathFadeRect;
	float deathAlpha;
	float fadeOutSpeed;

	sf::Sprite bloodPuddleSprite;
	sf::Sprite slashSprite;
	bool isAttacking;
	float attackTimer;
	
	std::vector<std::string> uiItemNames;
	std::vector<sf::RectangleShape> uiSlots;
	std::vector<sf::Sprite> uiIcons;
	std::vector<sf::Text> uiTexts;

public:
	Player(ResourceManager& res);
	void Update(float dt);
	void draw(sf::RenderWindow& window);
	void setPosition(float x, float y);
	sf::Vector2f getPosition();
	void ChangeIsMooving(bool IsMoove);
	void addItem(std::string name, int count = 1);
	void ToggleInventory();
	void removeItem(std::string name, int count = 1);
	void drawHUD(sf::RenderWindow& window, bool inShop = false);
	sf::FloatRect getBounds();
	void UpdateInventoryUI();
	void CheckDragStart(sf::Vector2f mousePosUI);
	void CheckDragEnd(sf::Vector2f mousePosUI);
	void addCoins(int count);
	bool spendCoins(int count); 
	int getCoins() const;
	const std::map<std::string, InventoryItem>& getInventory() const;
	bool upgradeItem(std::string name, int cost);
	void Attack(std::string weaponName);
	std::string GetActiveItem();
	bool GetIsDragging();
	bool IsInventoryOpen();
	bool isFacingRight();
	void TakeDamage(int dmg, float enemyX);
	int GetHealth();
	void Die();
	void UsePotion();
	bool IsDead();
	float GetDeathAlpha();
	void ResetFull();
	void SetHealth(int newHp);
	void ClearInventory();
	void setItemLevel(std::string name, int level);
};