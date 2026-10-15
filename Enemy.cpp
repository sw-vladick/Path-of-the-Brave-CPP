#include "Enemy.h"
#include "Config.h" 

Enemy::Enemy(ResourceManager& rm, std::string type, float startX, float startY)
{
	readyToThrow = false;
	throwFrames = 0;
	currentFrameXThrow = 0;
	this->startX = startX;
	this->startY = startY;
	enemyType = type;
	isDying = false;
	this->rm = &rm;
	dy = 0.f;
	OnGround = false;
	direction = -1;
	patrolTimer = 0.f;
	knockbackTimer = 0.f;
	fadeAlpha = 255.f;
	coolDown = 0.f;
	isAttacking = false;
	if (type == "Zombie") {
		maxHp = 100;
		damage = 10;
		speed = 100.f;
		deathFrames = 7;
		frameW = 77;
		frameH = 75;
		hitFrames = 5;

		runFrames = 9;
		sprite.setTexture(rm.textures["zombie"]); 
		currentFrameX = 0;
		animationTimer = 0.f;
		uvRect = sf::IntRect(0, 0, frameW, frameH);
		sprite.setTextureRect(uvRect);
	}
	else if (type == "Orc") {
		maxHp = 200; 
		damage = 25; 
		speed = 60.f; 
		deathFrames = 6;
		frameW = 84;
		frameH = 82;
		hitFrames = 3;
		sprite.setTexture(rm.textures["orc"]);
		runFrames = 12;
		currentFrameX = 0;
		animationTimer = 0.f;
		uvRect = sf::IntRect(0, 0, frameW, frameH);
		sprite.setTextureRect(uvRect);
	}
	else if(type == "mini") {
		maxHp = 70;
		damage = 10;
		speed = 450.f;
		deathFrames = 5;
		frameW = 106;
		frameH = 89;
		hitFrames = 5;
		sprite.setTexture(rm.textures["mini"]);
		runFrames = 12;
		currentFrameX = 0;
		animationTimer = 0.f;
		uvRect = sf::IntRect(0, 0, frameW, frameH);
		sprite.setTextureRect(uvRect);
	}
	else if (type == "wiking") {
		maxHp = 100;
		damage = 0;
		speed = 100.f;
		deathFrames = 6;
		frameW = 73;
		frameH = 71;
		hitFrames = 6;
		throwFrames = 9;
		sprite.setTexture(rm.textures["wiking"]);
		runFrames = 14;
		currentFrameX = 0;
		animationTimer = 0.f;
		uvRect = sf::IntRect(0, 0, frameW, frameH);
		sprite.setTextureRect(uvRect);
	}
	hp = maxHp; 


	sprite.setOrigin(frameW / 2.f, frameH / 2.f);
	sprite.setPosition(startX, startY);

	hpBarBack.setSize(sf::Vector2f(50.f, 5.f));
	hpBarBack.setFillColor(sf::Color::Red);
	hpBarBack.setOrigin(25.f, 2.5f);

	hpBarFront.setSize(sf::Vector2f(50.f, 5.f));
	hpBarFront.setFillColor(sf::Color::Green);
	hpBarFront.setOrigin(25.f, 2.5f);
}
void Enemy::Update(float dt, sf::Vector2f playerPos)
{
	if (isDying) {
		animationTimer += dt;
		if (animationTimer >= 0.15f) {
			animationTimer = 0.f;
			currentFrameX += 1;
			if (currentFrameX >= deathFrames) currentFrameX = deathFrames - 1;
			
			uvRect = sf::IntRect(currentFrameX * frameW, 0, frameW, frameH);
			sprite.setTextureRect(uvRect);
			
			
		}
		if (currentFrameX == deathFrames - 1) {
			fadeAlpha -= (255.f / 1.5f) * dt;
			if (fadeAlpha <= 0) fadeAlpha = 0;
			sprite.setColor(sf::Color(255, 255, 255, (sf::Uint8)fadeAlpha));
		}
		dy += Config::GRAVITY * dt;
		sprite.move(0.f, dy * dt);
		if (playerPos.x == 0 && playerPos.y == 0) return;
		sf::Vector2f pos = sprite.getPosition();
		float myGroundY = (enemyType == "mini") ? Config::GROUND_Y + 60.f : Config::GROUND_Y;

		if (pos.y >= myGroundY) {
			sprite.setPosition(pos.x, myGroundY);
			dy = 0;
			OnGround = true;
		}
		
		return;
	}
	dy += Config::GRAVITY * dt;
	sprite.move(0.f, dy * dt);
	sf::Vector2f pos = sprite.getPosition();
	float myGroundY = (enemyType == "mini") ? Config::GROUND_Y + 60.f : Config::GROUND_Y;

	if (pos.y >= myGroundY) {
		sprite.setPosition(pos.x, myGroundY);
		dy = 0;
		OnGround = true;
	}
	if (coolDown >= 0) {
		coolDown -= dt;
	}
	if (knockbackTimer > 0.f) {
		knockbackTimer -= dt;
		
		animationTimer += dt;

		if (animationTimer >= 0.05f) {
			animationTimer = 0;
			currentFrameX += 1;

			
			if (currentFrameX >= hitFrames) {
				currentFrameX = hitFrames - 1;
			}

			uvRect = sf::IntRect(currentFrameX * frameW, 0, frameW, frameH);
			sprite.setTextureRect(uvRect);
		}

		sprite.move(direction * 300.f * dt, 0.f);
	}
	else {
		if (enemyType == "wiking") {
			
			if (!isAttacking && std::abs(playerPos.x - pos.x) <= 600 && coolDown <= 0) {
				isAttacking = true;
				currentFrameX = 0;
				animationTimer = 0.f;
				sprite.setTexture(rm->textures["wiking_throw"]);
				frameW = 77;
				frameH = 70;
				direction = (pos.x < playerPos.x) ? 1 : -1;

				
				sprite.setScale(direction * 2.5f, 2.5f);
			}

			
			if (isAttacking) {
				animationTimer += dt;
				if (animationTimer >= 0.15f) {
					animationTimer = 0;
					currentFrameX++;

					if (currentFrameX == 6) {
						if (!readyToThrow) { 
							readyToThrow = true;

							
							rm->sounds["throw"].play();
						}
					}

					if (currentFrameX >= throwFrames) {
						isAttacking = false;
						coolDown = 3.0f;
						currentFrameX = 0;
						sprite.setTexture(rm->textures["wiking"]);
						frameW = 73;
						frameH = 71;
					}
					else {
						uvRect = sf::IntRect(currentFrameX * frameW, 0, frameW, frameH);
						sprite.setTextureRect(uvRect);
					}
				}
			}
			
			else if (!isAttacking && std::abs(playerPos.x - pos.x) > 500) {
				direction = (pos.x < playerPos.x) ? 1 : -1;
				sprite.setScale(direction * 2.5f, 2.5f); 
				sprite.move(direction * dt * speed, 0.f);
			}
			
			else if (!isAttacking && std::abs(playerPos.x - pos.x) <= 500) {
				direction = (pos.x < playerPos.x) ? 1 : -1;
				sprite.setScale(direction * 2.5f, 2.5f);

				
				currentFrameX = 0;
				animationTimer = 0.f;

				uvRect = sf::IntRect(0, 0, frameW, frameH);
				sprite.setTextureRect(uvRect);
			}
			sprite.setColor(sf::Color::White);
		}
		
		else {
			if (std::abs(playerPos.x - pos.x) >= 30) {
						if (pos.x < playerPos.x) {
							direction = 1;
							if (enemyType == "mini") {
								sprite.setScale(1.5f, 1.5f);
							}
							else {
								sprite.setScale(2.5f, 2.5f);
							}

						}
						else if (pos.x > playerPos.x) {
							direction = -1;
							if (enemyType == "mini") {
								sprite.setScale(-1.5f, 1.5f);
							}
							else {
								sprite.setScale(-2.5f, 2.5f);
							}


						}
					}
					sprite.setColor(sf::Color::White);
					if (enemyType == "Zombie") { sprite.setTexture(rm->textures["zombie"]); frameW = 77; frameH = 75; }
					else if (enemyType == "Orc") { sprite.setTexture(rm->textures["orc"]); frameW = 84; frameH = 82; }
					else if (enemyType == "mini") { sprite.setTexture(rm->textures["mini"]); frameW = 106; frameH = 89; }
					else if (enemyType == "wiking") { sprite.setTexture(rm->textures["wiking"]); frameW = 73; frameH = 71; }

					sprite.move(direction * dt * speed, 0.f);
		
					if (OnGround == true && playerPos.y < Config::GROUND_Y - 40.f) {
						dy = -850.f;
						OnGround = false;
					}
		}
		
	}
	

	pos = sprite.getPosition();
	hpBarBack.setPosition(pos.x, pos.y - 80.f);
	hpBarFront.setPosition(pos.x, pos.y - 80.f);
	if (!isAttacking) {
		if (!OnGround) {
			uvRect = sf::IntRect(7 * frameW, 0, frameW, frameH);
			sprite.setTextureRect(uvRect);
		}
		else {
			animationTimer += dt;
			if (animationTimer >= frameTime) {
				animationTimer = 0.f;
				currentFrameX += 1;
				if (currentFrameX >= runFrames) currentFrameX = 0;

				uvRect = sf::IntRect(currentFrameX * frameW, 0, frameW, frameH);
				sprite.setTextureRect(uvRect);
			}
		}
	}
}
void Enemy::Draw(sf::RenderWindow& window)
{
	if (hp > 0 || isDying) {
		window.draw(sprite);
	}

	if (hp > 0) {
		window.draw(hpBarBack);
		window.draw(hpBarFront);
	}
}

sf::FloatRect Enemy::getBounds() {
	sf::FloatRect box = sprite.getGlobalBounds();
	if (enemyType == "Zombie") {
		box.left += 45.f;
		box.width -= 65.f;
	}
	else if (enemyType == "Orc") {
		box.left += 45.f;
		box.width -= 65.f;
	}
	else if (enemyType == "mini") {
		box.left += 45.f;
		box.width -= 65.f;
	}
	else if (enemyType == "wiking") {
		box.left += 45.f;
		box.width -= 65.f;
	}
	return box;
}
bool Enemy::IsAlive() { return hp > 0; }

int Enemy::GetDamage()
{
	return damage;
}

float Enemy::CurrentX()
{
	return sprite.getPosition().x;
}

bool Enemy::GetDying()
{
	return isDying;
}

void Enemy::TakeDamage(int dmg, float playerX) {
	if (isDying) return;
	hp -= dmg;
	rm->sounds["hit"].play();
	if (hp <= 0) {
		hp = 0;
		isDying = true;
		rm->sounds["monster_die"].play();
		if (enemyType == "Zombie") {
			sprite.setTexture(rm->textures["zombie_death"]);
			sprite.setScale(direction * 4.2f, 4.2f);
			frameW = 77;
			frameH = 75;
			sprite.setOrigin(32.f, 20.f); 
		}
		if (enemyType == "Orc") {
			sprite.setTexture(rm->textures["orc_die"]);
			sprite.setScale(direction * 3.f, 3.f);

			frameW = 77;
			frameH = 77;

			sprite.setOrigin(frameW / 2.f, frameH / 2.f-8.f);
		}
		if (enemyType == "mini") {
			sprite.setTexture(rm->textures["mini_die"]);
			sprite.setScale(direction * 1.5f, 1.5f);

			frameW = 112;
			frameH = 86;

			sprite.setOrigin(frameW / 2.f, frameH / 2.f);
		}
		if (enemyType == "wiking") {
			sprite.setTexture(rm->textures["wiking_die"]);
			sprite.setScale(direction * 3.0f, 3.0f);

			frameW = 87;
			frameH = 71;

			sprite.setOrigin(frameW / 2.f, frameH / 2.f);
		}
		currentFrameX = 0;
		animationTimer = 0.f;
		sprite.setColor(sf::Color::White);
		return;
	}
	knockbackTimer = 0.3f;

	sf::Vector2f posZ = sprite.getPosition();
	
	if (enemyType == "Zombie") {
		sprite.setTexture(rm->textures["zombie_hurt"]);
		frameW = 77;
		frameH = 75;
	}
	if (enemyType == "Orc") {
		sprite.setTexture(rm->textures["orc_hurt"]);
		frameW = 86;
		frameH = 85;
	}
	if (enemyType == "mini") {
		sprite.setTexture(rm->textures["mini_hurt"]);
		frameW = 108;
		frameH = 90;
	}
	if (enemyType == "wiking") {
		sprite.setTexture(rm->textures["wiking_hurt"]);
		frameW = 86;
		frameH = 75;
	}
	animationTimer = 0;
	currentFrameX = 0;
	if (posZ.x >= playerX) {
		direction = 1;
	}
	else if (posZ.x <= playerX) {
		direction = -1;
	}
	sprite.setColor(sf::Color::Red);
	float hpPercent = (float)hp / maxHp;
	hpBarFront.setSize(sf::Vector2f(50.f * hpPercent, 5.f));
}

float Enemy::CurrentY() {
	return sprite.getPosition().y;
}

void Enemy::Reset()
{
	hp = maxHp;
	isDying = false;
	sprite.setColor(sf::Color::White);
	animationTimer = patrolTimer = knockbackTimer = 0;
	sprite.setPosition(startX, startY);
}

bool Enemy::CanThrow()
{
	if (readyToThrow) {
		readyToThrow = false; 
		return true;          
	}
	return false;
}
