#include "Player.h"
#include "Menu.h"
#include "Config.h"

Player::Player(ResourceManager& res) : quickBar(res)
{
	rm = &res; 
	isDead = false;
	deathAlpha = 0.f;
	fadeOutSpeed = 255.f / 3.f;
	deathFadeRect.setSize(sf::Vector2f(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height));
	deathFadeRect.setFillColor(sf::Color(0, 0, 0, 0));
	bloodPuddleSprite.setTexture(rm->textures["blood"]);
	bloodPuddleSprite.setOrigin(rm->textures["blood"].getSize().x / 2.f, rm->textures["blood"].getSize().y / 2.f);
	bloodPuddleSprite.setScale(0.7f, 0.7f);

	
	invincibilityTimer = 0.f;
	coins = 0;
	sprite.setTexture(rm->textures["idle"]);

	slotRect.setSize(sf::Vector2f(50.f, 50.f));
	slotRect.setFillColor(sf::Color(100, 100, 100, 200));
	slotRect.setOutlineThickness(2.f);
	slotRect.setOutlineColor(sf::Color::White);

	inventoryText.setFont(rm->font);
	inventoryText.setCharacterSize(15);
	inventoryText.setFillColor(sf::Color::Yellow);
	inventoryText.setOutlineThickness(1);
	inventoryText.setOutlineColor(sf::Color::Black);

	potionCooldownTimer = 0.f;
	maxHealth = 100;
	currentHealth = 100;

	hpBarBack.setSize(sf::Vector2f(200.f, 20.f));
	hpBarBack.setFillColor(sf::Color::Red);
	hpBarBack.setPosition(20.f, 20.f);

	hpBarFront.setSize(sf::Vector2f(200.f, 20.f));
	hpBarFront.setFillColor(sf::Color::Green);
	hpBarFront.setPosition(20.f, 20.f);
	showInventory = false;

	inventoryBox.setSize(sf::Vector2f(1620.f, 780.f));
	inventoryBox.setFillColor(sf::Color(0, 0, 0, 200));
	inventoryBox.setOrigin(sf::Vector2f(710.f, 390.f));
	inventoryBox.setPosition(860.f, 540.f);
	inventoryBox.setOutlineThickness(2);
	inventoryBox.setOutlineColor(sf::Color::White);


	frameCount = 2;
	int totalIdleWidth = rm->textures["idle"].getSize().x;
	frameWidth = totalIdleWidth / frameCount;
	frameHeight = rm->textures["idle"].getSize().y;

	currentFrame = sf::IntRect(0, 0, frameWidth, frameHeight);
	sprite.setTextureRect(currentFrame);
	sprite.setOrigin(frameWidth / 2, frameHeight / 2);
	sprite.setPosition(Config::PLAYER_START_X, Config::PLAYER_START_Y);

	animationTimer = 0.0f;
	animationSpeed = 0.5f;
	currentFrameIndex = 0;
	IsMoving = false;
	dy = 0;
	OnGround = true;
	run = 350.f;
	isAttacking = false;
	attackTimer = 0.f;

	isDragging = false;
	draggedItem = "";
	currentState = IDLE;
	knockbackDirection = 1;
	checkItem = -1;

	sprite.setScale(1.5f, 1.5f);
}

void Player::UpdateInventoryUI()
{
	uiSlots.clear();
	uiIcons.clear();
	uiTexts.clear();
	uiItemNames.clear();

	float startX = 190.f;
	float startY = 180.f;
	float slotSize = 50.f;
	float padding = 10.f;
	int columns = 4;
	int i = 0;

	for (auto const& item : inventory) {
		std::string name = item.first;
		int count = item.second.count;
		int lvl = item.second.level;

		float x = startX + (i % columns) * (slotSize + padding);
		float y = startY + (i / columns) * (slotSize + padding);

		slotRect.setPosition(x, y);
		uiSlots.push_back(slotRect);

		iconSprite.setTexture(rm->textures[name]);
		sf::Vector2u texSize = iconSprite.getTexture()->getSize();
		iconSprite.setScale(slotSize / texSize.x, slotSize / texSize.y);
		iconSprite.setPosition(x, y);
		uiIcons.push_back(iconSprite);

		if (item.second.isUpgradable && lvl > 0) {
			inventoryText.setString("Lv." + std::to_string(lvl));
			inventoryText.setFillColor(sf::Color::Cyan);
		}
		else {
			inventoryText.setString(std::to_string(count));
			inventoryText.setFillColor(sf::Color::Yellow);
		}
		inventoryText.setPosition(x + slotSize - 20, y + slotSize - 20);
		uiTexts.push_back(inventoryText);
		uiItemNames.push_back(name);
		i++;

	}
}

void Player::Update(float dt)
{
	if (isDead) {
		deathAlpha += fadeOutSpeed * dt;
		if (deathAlpha >= 255) deathAlpha = 255;
		deathFadeRect.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)deathAlpha));
		dy += Config::GRAVITY * dt;
		sprite.move(0.f, dy * dt);

		sf::Vector2f where = sprite.getPosition();

		if (where.y > Config::GROUND_Y) {
			sprite.setPosition(where.x, Config::GROUND_Y);
			dy = 0;
			OnGround = true;
		}
		return;
	}
	if (potionCooldownTimer > 0.f) potionCooldownTimer -= dt;
	if (currentHealth < 0) {
		currentHealth = 0;
	}
	float hpPercent = (float)currentHealth / maxHealth;
	bool isSprinting = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
	run = isSprinting ? 500.f : 350.f;

	IsMoving = false;
	if (invincibilityTimer > 0) {
		invincibilityTimer -= dt;
		sprite.move(knockbackDirection * 300.f * dt, 0.f);
		IsMoving = true;
	}
	else {
		sprite.setColor(sf::Color::White);
		quickBar.update();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			sprite.move(run * dt, 0.f);
			sprite.setScale(1.5f, 1.5f);
			IsMoving = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			sprite.move(-run * dt, 0.f);
			sprite.setScale(-1.5f, 1.5f);
			IsMoving = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && OnGround == true) {
			dy = -1000.f;
			rm->sounds["jump"].play();
			OnGround = false;
			IsMoving = true;
		}
	}
	dy += Config::GRAVITY * dt;
	sprite.move(0.f, dy * dt);

	sf::Vector2f where = sprite.getPosition();

	if (where.y > Config::GROUND_Y) {
		sprite.setPosition(where.x, Config::GROUND_Y);
		dy = 0;
		OnGround = true;
	}

	sf::FloatRect size = sprite.getGlobalBounds();
	float halfwidth = size.width / 2.f;
	if (where.x - halfwidth < 0) {
		sprite.setPosition(halfwidth, where.y);
	}
	if (where.x + halfwidth > Config::MAP_WIDTH) {
		sprite.setPosition(Config::MAP_WIDTH - halfwidth, where.y);
	}

	PlayerState newState;
	if (OnGround == false) newState = JUMP;
	else if (IsMoving == true) newState = isSprinting ? FRUN : RUN;
	else newState = IDLE;

	if (currentState != newState) {
		currentState = newState;
		if (currentState == IDLE) {
			sprite.setTexture(rm->textures["idle"]);
			frameCount = 2; animationSpeed = 0.4f;
		}
		else if (currentState == RUN) {
			sprite.setTexture(rm->textures["run"]);
			frameCount = 10; animationSpeed = 0.15f;
		}
		else if (currentState == FRUN) {
			sprite.setTexture(rm->textures["run"]);
			frameCount = 10; animationSpeed = 0.08f;
		}
		else if (currentState == JUMP) {
			sprite.setTexture(rm->textures["jump"]);
			frameCount = 1;
		}

		frameWidth = sprite.getTexture()->getSize().x / frameCount;
		frameHeight = sprite.getTexture()->getSize().y;
		currentFrameIndex = 0;
		sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
	}

	animationTimer += dt;
	if (animationTimer >= animationSpeed) {
		animationTimer = 0;
		currentFrameIndex++;
		if (OnGround && std::abs(animationSpeed) > 0) {
			if (currentFrameIndex == 1 || currentFrameIndex == 5) {
				rm->sounds["walk"].play();
			}
		}
		if (currentFrameIndex >= frameCount) currentFrameIndex = 0;
	}

	int gap = 13;
	currentFrame.left = (currentFrameIndex * frameWidth);
	currentFrame.top = 0;
	currentFrame.width = frameWidth - gap;
	currentFrame.height = frameHeight; 
	sprite.setTextureRect(currentFrame);

	if (isAttacking) {
		attackTimer += dt;
		if (attackTimer >= 0.15f) isAttacking = false;

		if (sprite.getScale().x > 0) {
			
			slashSprite.setPosition(sprite.getPosition().x + 100.f, sprite.getPosition().y);
			slashSprite.setScale(0.5f, 0.5f);
		}
		else if (sprite.getScale().x < 0) {
			
			slashSprite.setPosition(sprite.getPosition().x - 100.f, sprite.getPosition().y);
			slashSprite.setScale(-0.5f, 0.5f);
		}
	}
	hpBarFront.setSize(sf::Vector2f(200.f * hpPercent, 20.f));
}

void Player::draw(sf::RenderWindow& window) { 
	if (isDead) {
		window.draw(bloodPuddleSprite);
	}

	window.draw(sprite);

	if (isAttacking) {
		window.draw(slashSprite);
	}
}
void Player::setPosition(float x, float y) { sprite.setPosition(x, y); }
sf::Vector2f Player::getPosition() { return sprite.getPosition(); }
void Player::ChangeIsMooving(bool IsMoove) { IsMoving = IsMoove; }
sf::FloatRect Player::getBounds() {
	sf::FloatRect box = sprite.getGlobalBounds();
	box.left += 45.f;
	box.width -= 65.f;
	box.top += 5.f;
	box.height -= 5.f;
	return box;
}

void Player::addItem(std::string name, int count) {

	if (inventory.find(name) == inventory.end()) {
		inventory[name].count = 0;
		inventory[name].level = 0; 

	
		if (name == "Sword" || name == "Armor") {
			inventory[name].isUpgradable = true;
		}
		else {
			inventory[name].isUpgradable = false; 
		}
	}


	inventory[name].count += count;

	if (showInventory) UpdateInventoryUI();
	quickBar.AddItem(name);
}

void Player::removeItem(std::string name, int count) {
	if (inventory.count(name)) {
		inventory[name].count -= count;
		if (inventory[name].count <= 0) inventory.erase(name);
		if (showInventory) UpdateInventoryUI();
	}
}

void Player::drawHUD(sf::RenderWindow& window, bool inShop)
{
	if (!inShop || showInventory) {
		window.draw(hpBarBack);
		window.draw(hpBarFront);
		quickBar.drawHUD(window, inventory);
	}

	sf::Text coinText;
	coinText.setFont(rm->font); 
	coinText.setCharacterSize(24);
	coinText.setFillColor(sf::Color::Yellow);
	coinText.setOutlineThickness(2);
	coinText.setString("Coins: " + std::to_string(coins));
	coinText.setPosition(20.f, 50.f);
	window.draw(coinText);
	
	if (showInventory) {
		window.draw(inventoryBox);
		for (auto& s : uiSlots) window.draw(s);
		for (auto& ic : uiIcons) window.draw(ic);
		for (auto& t : uiTexts) window.draw(t);
	}

	if (isDragging) {
		
		sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
		sf::Vector2f mousePosUI = window.mapPixelToCoords(pixelPos, window.getDefaultView());

		draggedSprite.setPosition(mousePosUI);
		window.draw(draggedSprite);
	}
	if (potionCooldownTimer > 0.f) {
		
		sf::Text cdText;

		cdText.setFont(rm->font);

		cdText.setCharacterSize(20);
		cdText.setFillColor(sf::Color::Cyan);
		cdText.setOutlineThickness(2.f);
		cdText.setOutlineColor(sf::Color::Black);

		std::string timeStr = std::to_string((int)potionCooldownTimer + 1);
		cdText.setString("Potion CD: " + timeStr + "s");

		cdText.setPosition(240.f, 18.f);

		window.draw(cdText);
	}
	if (isDead) {
		window.draw(deathFadeRect);
	}
}

void Player::ToggleInventory()
{
	showInventory = !showInventory;
	if (showInventory) {
		UpdateInventoryUI(); 
	}
}

void Player::CheckDragStart(sf::Vector2f mousePosUI)
{

	checkItem = quickBar.GetSlotIndex(mousePosUI);

	if (checkItem != -1 && quickBar.GetCurrentItem(checkItem) != "") {
		isDragging = true;
		draggedItem = quickBar.GetCurrentItem(checkItem);

		quickBar.ClearSlot(checkItem); 

		draggedSprite.setTexture(rm->textures[draggedItem]);
		draggedSprite.setOrigin(draggedSprite.getTexture()->getSize().x / 2.f, draggedSprite.getTexture()->getSize().y / 2.f);
		return; 
	}


	if (showInventory) {
		for (size_t i = 0; i < uiSlots.size(); i++) {
			if (uiSlots[i].getGlobalBounds().contains(mousePosUI)) {
				isDragging = true;
				draggedItem = uiItemNames[i]; 
				checkItem = -1;             

				draggedSprite.setTexture(rm->textures[draggedItem]);
				draggedSprite.setOrigin(draggedSprite.getTexture()->getSize().x / 2.f, draggedSprite.getTexture()->getSize().y / 2.f);
				return;
			}
		}
	}
}

void Player::CheckDragEnd(sf::Vector2f mousePosUI)
{
	if (!isDragging) return;

	int dropSlot = quickBar.GetSlotIndex(mousePosUI);

	if (dropSlot != -1) {
		
		for (int i = 0; i < 4; i++) {
			if (quickBar.GetCurrentItem(i) == draggedItem) {
				quickBar.ClearSlot(i);
			}
		}
		
		quickBar.SetSlot(dropSlot, draggedItem);
	}
	else {
		
	}


	isDragging = false;
	draggedItem = "";
}

void Player::addCoins(int count) { coins += count; }

bool Player::spendCoins(int count) {
	if (coins >= count) {
		coins -= count;
		return true;
	}
	return false;
}

int Player::getCoins() const { return coins; }

const std::map<std::string, InventoryItem>& Player::getInventory() const
{
	return inventory;
}

bool Player::upgradeItem(std::string name, int cost) {
	
	if (inventory.count(name) && inventory[name].level < 5) {
		if (coins >= cost) {
			coins -= cost;           
			inventory[name].level++;    

			if (showInventory) UpdateInventoryUI();
			return true;
		}
	}
	return false;
}

void Player::Attack(std::string weaponName) {
	rm->sounds["vzmah"].play();
	isAttacking = true;
	attackTimer = 0.f;

	std::string slashName = "slash_" + weaponName;


	if (rm->textures.count(slashName) > 0) {
		slashSprite.setTexture(rm->textures[slashName]);


		sf::Vector2u texSize = slashSprite.getTexture()->getSize();
		slashSprite.setOrigin(texSize.x / 2.f, texSize.y / 2.f);
	}
}

std::string Player::GetActiveItem()
{
	return quickBar.UseItem();
}

bool Player::GetIsDragging() {
	return isDragging;
}

bool Player::IsInventoryOpen() {
	return showInventory;
}

bool Player::isFacingRight() {
	return sprite.getScale().x > 0;
}

void Player::TakeDamage(int dmg, float enemyX) {
	if (invincibilityTimer > 0.f) return;
	rm->sounds["hitgg"].play();
	currentHealth -= dmg;
	invincibilityTimer = 0.5f;

	sf::Vector2f pos = sprite.getPosition();

	if (pos.x > enemyX) {
		knockbackDirection = 1;
	}
	else {
		knockbackDirection = -1;
	}

	sprite.setColor(sf::Color::Red);
}

int Player::GetHealth()
{
	return currentHealth;
}

void Player::Die()
{
	if (isDead) return;
	rm->sounds["death"].play();
	isDead = true;
	sf::Vector2f pos = getPosition();
	bloodPuddleSprite.setPosition(pos.x, Config::GROUND_Y + 15.f);
	sprite.setTexture(rm->textures["idle"]);
	int idleWidth = rm->textures["idle"].getSize().x / 2;
	int idleHeight = rm->textures["idle"].getSize().y;
	currentFrame = sf::IntRect(0, 0, idleWidth, idleHeight);
	sprite.setOrigin(idleWidth / 2.f, idleHeight / 2.f);
	
	sprite.setTextureRect(currentFrame);
}

void Player::UsePotion()
{
	if (inventory.find("Potion") == inventory.end() || inventory["Potion"].count <= 0) return;
	if (potionCooldownTimer > 0) return;
	rm->sounds["drink"].play();
	currentHealth += 30;
	if (currentHealth >= maxHealth) currentHealth = maxHealth;
	potionCooldownTimer = 30.f;
	removeItem("Potion", 1);
}

bool Player::IsDead()
{
	return isDead;
}

float Player::GetDeathAlpha()
{
	return deathAlpha;
}

void Player::ResetFull()
{
	isDead = false;  currentHealth = maxHealth; deathAlpha = 0.f;  coins = 0;  inventory.clear();
	potionCooldownTimer = 0;
	sprite.setPosition(Config::PLAYER_START_X, Config::PLAYER_START_Y);
}

void Player::SetHealth(int newHp) {
	currentHealth = newHp;
	if (currentHealth > maxHealth) currentHealth = maxHealth;
}

void Player::ClearInventory()
{
	inventory.clear();
	for (int i = 0; i < 4; i++) {
		quickBar.ClearSlot(i);
	}
}

void Player::setItemLevel(std::string name, int level)
{
	inventory[name].level = level;
}
