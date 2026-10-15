#pragma once
#include <SFML/Graphics.hpp>
#include "string"
#include "ResourceManager.h"

class Enemy
{
private:
	sf::Sprite sprite;
	bool isAttacking;
	int hp;
	int maxHp;
	int damage;
	float speed;
	bool isDying;
	float fadeAlpha;
	std::string enemyType;
	float startX;
	float startY;
	ResourceManager* rm;
	float dy;
	bool OnGround;
	int deathFrames;
	int frameW;
	int frameH;
	int hitFrames;
	int runFrames;
	int throwFrames;
	int currentFrameXThrow;
	float coolDown;
	bool readyToThrow;

	sf::IntRect uvRect;
	float animationTimer;
	int currentFrameX;

	const int frameWidth = 77;
	const int frameHeight = 75;
	const float frameTime = 0.1f;

	int direction;      
	float patrolTimer;   
	float knockbackTimer;

	sf::RectangleShape hpBarBack;
	sf::RectangleShape hpBarFront;

public:
	Enemy(ResourceManager& rm, std::string type, float startX, float startY);
	void Update(float dt, sf::Vector2f playerPos);
	void Draw(sf::RenderWindow& window);
	std::string entityId;
	
	sf::FloatRect getBounds();
	void TakeDamage(int dmg, float playerX);
	bool IsAlive();
	int GetDamage();
	float CurrentX();
	bool GetDying();
	float CurrentY();
	void Reset();
	bool CanThrow();
	
};