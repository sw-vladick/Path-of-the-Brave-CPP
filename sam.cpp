#include "SFML/Graphics.hpp"
#include "Player.h"
#include "Menu.h"
#include "Chest.h"
#include "Item.h"
#include "ResourceManager.h"
#include "Config.h"
#include <vector>
#include "Enemy.h"	
#include <cstdlib>
#include <ctime>
#include <chrono>
#include "DatabaseManager.h"

void SpawnLevel1(std::vector<Enemy>& enemies, ResourceManager& rm) {
	for (int i = 0; i < 10; i++) {
		enemies.emplace_back(rm, "Zombie", 1500 + (500 * i), Config::GROUND_Y);
		enemies.back().entityId = "lvl1_zombie_" + std::to_string(i);
	}
	for (int i = 0; i < 4; i++) {
		enemies.emplace_back(rm, "Orc", 1990 + (1000 * i), Config::GROUND_Y);
		enemies.back().entityId = "lvl1_orc_" + std::to_string(i);
	}
	enemies.emplace_back(rm, "wiking", 3500, Config::GROUND_Y);
	enemies.back().entityId = "lvl1_wiking_boss";
}

void SpawnWave1(std::vector<Enemy>& enemies, ResourceManager& rm) {
	
	for (int i = 0; i < 9; i++) {
		enemies.emplace_back(rm, "Zombie", 5000 + (500 * i), Config::GROUND_Y);
		enemies.back().entityId = "wave1_zombie_" + std::to_string(i);
	}
	for (int i = 0; i < 4; i++) {
		enemies.emplace_back(rm, "Orc", 5000 + (1000 * i), Config::GROUND_Y);
		enemies.back().entityId = "wave1_orc_" + std::to_string(i);
	}
	for (int i = 0; i < 3; i++) {
		enemies.emplace_back(rm, "wiking", 5000 + (1000 * i), Config::GROUND_Y);
		enemies.back().entityId = "wave1_wiking_" + std::to_string(i);
	}
}

void SpawnWave2(std::vector<Enemy>& enemies, ResourceManager& rm) {
	
	for (int i = 0; i < 6; i++) {
		enemies.emplace_back(rm, "mini", 6500 + (500 * i), Config::GROUND_Y);
		enemies.back().entityId = "wave2_mini_" + std::to_string(i);
	}
}

struct Projectile {
	sf::Sprite sprite;
	float vx;
	float vy;
	bool isActive = true;
	Projectile(sf::Texture& tex, float startX, float startY, float velocityX, float velocityY) {
		sprite.setTexture(tex);
		sprite.setScale(2.5f, 2.5f);
		sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
		sprite.setPosition(startX, startY);
		vx = velocityX;
		vy = velocityY;
		isActive = true;
	}

	void Update(float dt) {
		
		vy += Config::GRAVITY * dt;

		sprite.move(vx * dt, vy * dt);

		sprite.rotate(800.f * dt);

		if (sprite.getPosition().y >= Config::GROUND_Y) {
			isActive = false;
		}
	}
};

int main()
{
	ResourceManager rm;
	rm.LoadAll();
	rm.textures["background"].setRepeated(true);

	DatabaseManager dbManager;
	dbManager.InitDatabase();

	srand(time(0));
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(desktop, "Path of the Brave", sf::Style::Fullscreen);

	sf::Music bgMusic;
	if (bgMusic.openFromFile("bg_music.ogg")) { 
		bgMusic.setLoop(true);
		bgMusic.setVolume(20.f); 
		bgMusic.play();          
	}

	Player player(rm);
	std::vector<Enemy> enemies;
	std::vector<Projectile> axes;
	sf::Clock clock;

	sf::Text gameOverText("GAME OVER", rm.font, 100);
	gameOverText.setFillColor(sf::Color::Red);
	gameOverText.setOutlineThickness(5.f);
	gameOverText.setOutlineColor(sf::Color::Black);
	sf::FloatRect textRect = gameOverText.getLocalBounds();
	gameOverText.setOrigin(textRect.width / 2.f, textRect.height / 2.f);
	gameOverText.setPosition(desktop.width / 2.f, desktop.height / 2.f);
	float gameOverTimer = 0.f;

	sf::Sprite spback;
	spback.setTexture(rm.textures["background"]);
	int bgHeight = rm.textures["background"].getSize().y;
	spback.setTextureRect(sf::IntRect(0, 0, Config::MAP_WIDTH, bgHeight));
	float scale = 1080.f / bgHeight;
	spback.setScale(scale, scale);

	sf::View view = window.getDefaultView();
	bool StillMenu = true;
	sf::Vector2u sizew = window.getSize();

	Menu menu(sizew.x, sizew.y, rm.font);
	Chest chest(rm.textures["chest"]);
	chest.entityId = "chest_start_1";
	chest.SetPosition(400.f);
	bool chestIsActive = true;

	sf::Sprite portal;
	portal.setTexture(rm.textures["portal"]);
	sf::Vector2u portalSize = rm.textures["portal"].getSize();
	portal.setOrigin(portalSize.x / 2.f, portalSize.y);
	portal.setScale(1.0f, 1.0f);
	portal.setPosition(9800.f, Config::GROUND_Y+110);


	bool isFading = false;
	float fadeAlpha = 0.f;
	sf::RectangleShape fadeScreen(sf::Vector2f(sizew.x, sizew.y));
	fadeScreen.setFillColor(sf::Color(0, 0, 0, 0));
	bool inShop = false;
	int currentLevel = 1;


	sf::Sprite shopBg;
	sf::RectangleShape fallbackBg(sf::Vector2f(sizew.x, sizew.y));
	fallbackBg.setFillColor(sf::Color(40, 40, 40));

	sf::Vector2u shopSize = rm.textures["shop_bg"].getSize();
	if (shopSize.x > 0) {
		shopBg.setTexture(rm.textures["shop_bg"]);
		shopBg.setScale((float)sizew.x / shopSize.x, (float)sizew.y / shopSize.y);
	}


	sf::RectangleShape btnNext(sf::Vector2f(250.f, 70.f));
	btnNext.setFillColor(sf::Color(139, 0, 0));
	btnNext.setOutlineThickness(3.f);
	btnNext.setOutlineColor(sf::Color::White);
	btnNext.setPosition(sizew.x / 2.f - 125.f, sizew.y - 120.f);

	sf::Text btnNextText("NEXT LEVEL", rm.font, 32);
	btnNextText.setFillColor(sf::Color::White);
	btnNextText.setPosition(btnNext.getPosition().x + 17.f, btnNext.getPosition().y + 10.f);

	std::vector<Item> droppedItems;

	int currentShopTab = 0;

	sf::RectangleShape btnTabShop(sf::Vector2f(300.f, 60.f));
	btnTabShop.setPosition(sizew.x / 2.f - 320.f, 100.f);
	btnTabShop.setOutlineThickness(3.f);

	sf::RectangleShape btnTabUpgrade(sf::Vector2f(300.f, 60.f));
	btnTabUpgrade.setPosition(sizew.x / 2.f + 20.f, 100.f);
	btnTabUpgrade.setOutlineThickness(3.f);

	sf::Text textTabShop("BUY ITEMS", rm.font, 28);
	textTabShop.setPosition(btnTabShop.getPosition().x + 70.f, btnTabShop.getPosition().y + 12.f);

	sf::Text textTabUpgrade("UPGRADE", rm.font, 28);
	textTabUpgrade.setPosition(btnTabUpgrade.getPosition().x + 25.f, btnTabUpgrade.getPosition().y + 12.f);

	
	sf::RectangleShape shopPanel(sf::Vector2f(1000.f, 600.f));
	shopPanel.setFillColor(sf::Color(20, 20, 20, 230));
	shopPanel.setOutlineThickness(4.f);
	shopPanel.setOutlineColor(sf::Color(100, 100, 100));
	shopPanel.setPosition(sizew.x / 2.f - 500.f, 160.f);

	Item Coin;

	bool wave1Spawned = false;
	bool wave2Spawned = false;

	int currentSaveSlot = 1;
	int vol = 100;
	int pKey = sf::Keyboard::H;

	if (dbManager.LoadSettings(vol, pKey)) {
		menu.SetVolume(vol);
		menu.SetPotionKey(static_cast<sf::Keyboard::Key>(pKey));
		sf::Listener::setGlobalVolume(static_cast<float>(vol));
	}
	while (window.isOpen()) {
		window.clear();
		float dt = clock.restart().asSeconds();
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::LostFocus) StillMenu = true;

			if (event.type == sf::Event::KeyReleased) {
				if (StillMenu && menu.IsSettingsOpen()) {
					if (menu.IsWaitingForKey()) {
						if (event.key.code != sf::Keyboard::Escape) {
							menu.SetPotionKey(event.key.code);
							menu.SetWaitingForKey(false);
						}
						continue;
					}
					else {
						if (event.key.code == sf::Keyboard::Right) {
							int v = menu.GetVolume() + 10;
							if (v > 100) v = 100;
							menu.SetVolume(v);
							sf::Listener::setGlobalVolume(v);
						}
						if (event.key.code == sf::Keyboard::Left) {
							int v = menu.GetVolume() - 10;
							if (v < 0) v = 0;
							menu.SetVolume(v);
							sf::Listener::setGlobalVolume(v);
						}
					}
				}
				if (!StillMenu && !inShop && !player.IsDead()) {
					if (event.key.code == menu.GetPotionKey()) {
						player.UsePotion();
					}
				}
				if (event.key.code == sf::Keyboard::F5) {
					dbManager.SavePlayer(currentSaveSlot, player.GetHealth(), player.getPosition().x, player.getPosition().y, currentLevel, player.getCoins());
					std::vector<DBItem> itemsToSave;
					for (const auto& pair : player.getInventory()) {
						DBItem dbItem;
						dbItem.name = pair.first;
						
						dbItem.count = pair.second.count;
						dbItem.level = pair.second.level;
						itemsToSave.push_back(dbItem);
					}
					
					dbManager.SaveInventory(currentSaveSlot, itemsToSave);
					dbManager.SaveEntity(currentSaveSlot, chest.entityId, chestIsActive ? 0 : 1);

					
					dbManager.SaveEntity(currentSaveSlot, "wave1_trigger", wave1Spawned ? 1 : 0);
					dbManager.SaveEntity(currentSaveSlot, "wave2_trigger", wave2Spawned ? 1 : 0);

					
					for (auto& enemy : enemies) {
						dbManager.SaveEntity(currentSaveSlot, enemy.entityId, enemy.IsAlive() ? 0 : 1);
					}
					std::cout << "Game & World Saved!\n";
				}
				if (event.key.code == sf::Keyboard::Escape) {
					if (menu.IsSettingsOpen()) {
						dbManager.SaveSettings(menu.GetVolume(), static_cast<int>(menu.GetPotionKey()));
						menu.SetSettingsOpen(false);
					}
					rm.sounds["menu"].play();

					if (StillMenu) {
						if (menu.IsSettingsOpen()) {
							menu.SetSettingsOpen(false); 
						}
						else if (menu.IsSaveMenuOpen()) {
							menu.SetSaveMenuOpen(false); 
						}
						else {
							
							if (menu.GetPaused()) {
								StillMenu = false;  
								menu.SetPaused(false);
							}
							else {
								window.close();        
							}
						}
					}
					else if (!inShop) {
						
						StillMenu = true;     
						menu.SetPaused(true);  
					}
				}

				if (event.key.code == sf::Keyboard::I && !StillMenu) {
					player.ToggleInventory();
				}
			}

			sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
			sf::Vector2f mousePosUI = window.mapPixelToCoords(pixelPos, window.getDefaultView());
			sf::Vector2f mousePosWorld = window.mapPixelToCoords(pixelPos, view);

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

				player.CheckDragStart(mousePosUI);

				if (inShop) {
					
					if (btnNext.getGlobalBounds().contains(mousePosUI)) {
						inShop = false;
						chestIsActive = true;
						chest.SetPosition(400.f);
						player.setPosition(200.f, Config::GROUND_Y);
						currentLevel++;
					}
					
					else if (btnTabShop.getGlobalBounds().contains(mousePosUI)) {
						currentShopTab = 0; 
					}
					else if (btnTabUpgrade.getGlobalBounds().contains(mousePosUI)) {
						currentShopTab = 1;
					}
					
					else if (currentShopTab == 1) {
						const auto& inv = player.getInventory();
						float startX = shopPanel.getPosition().x + 20.f;
						float startY = shopPanel.getPosition().y + 20.f;
						int itemIndex = 0;

						for (const auto& pair : inv) {
							if (pair.second.isUpgradable) {
								sf::FloatRect rowBounds(startX, startY + (itemIndex * 90.f), 960.f, 80.f);
								
								if (rowBounds.contains(mousePosUI)) {
									int upgradeCost = (pair.second.level + 1) * 50;
									player.upgradeItem(pair.first, upgradeCost);
								}
								itemIndex++;
							}
						}
					}
					

				}
				
				else if (StillMenu) {
					if (menu.IsSettingsOpen()) {
						if (menu.IsWaitingForKey()) continue;

						int clickedBtn = menu.GetClickedSettingsButton(mousePosUI);
						if (clickedBtn == 1) { 
							menu.SetWaitingForKey(true);
						}
						else if (clickedBtn == 2) { 
							if (menu.IsSettingsOpen()) {
								dbManager.SaveSettings(menu.GetVolume(), static_cast<int>(menu.GetPotionKey()));
								menu.SetSettingsOpen(false);
							}
							menu.SetSettingsOpen(false);
						}
					}
					else if (!menu.IsSaveMenuOpen() && !menu.IsSettingsOpen()) {
						if (menu.isMouseOverSelectedItem(mousePosUI)) {

							
							if (menu.GetIndex() == 0) {

								if (menu.GetPaused()) {
									StillMenu = false;
									menu.SetPaused(false);
								}
								else {
									menu.SetSaveMenuOpen(true);
									std::vector<SaveSlotInfo> slots = dbManager.GetSaveSlots();
									menu.UpdateSaveSlots(slots);
								}
							}

							if (menu.GetIndex() == 1) {
								menu.SetSettingsOpen(true);
							}

							if (menu.GetIndex() == 2) {
								window.close();
							}
						}
					}
					
					else {
						
						int clickedDelete = menu.GetClickedDeleteButton(mousePosUI);

						if (clickedDelete > 0) {
							rm.sounds["hitgg"].play();
							dbManager.DeleteSaveSlot(clickedDelete);

							std::vector<SaveSlotInfo> slots = dbManager.GetSaveSlots();
							menu.UpdateSaveSlots(slots);
						}
						else {
							int clickedSlot = menu.GetClickedSlot(mousePosUI);

							if (clickedSlot > 0) {
								currentSaveSlot = clickedSlot;

								int loadedHp = 100;
								float loadedX = 200.f;
								float loadedY = Config::GROUND_Y;
								int loadedLevel = 1;
								int loadedCoins = 0;

								if (dbManager.LoadPlayer(clickedSlot, loadedHp, loadedX, loadedY, loadedLevel, loadedCoins)) {
								
									player.SetHealth(loadedHp);
									player.setPosition(loadedX, loadedY);
									currentLevel = loadedLevel;
									player.ClearInventory();
									player.addCoins(loadedCoins);

									std::vector<DBItem> loadedItems = dbManager.LoadInventory(clickedSlot);
									for (const auto& item : loadedItems) {
										player.addItem(item.name, item.count);
										player.setItemLevel(item.name, item.level);
									}

									int chestState = dbManager.GetEntityState(clickedSlot, chest.entityId);
									if (chestState == 1) {
										chestIsActive = false;
										chest.ChestOpen();

										enemies.clear();
										std::vector<Enemy> tempEnemies;
										SpawnLevel1(tempEnemies, rm);

										for (auto& e : tempEnemies) {
											if (dbManager.GetEntityState(clickedSlot, e.entityId) == 0) {
												enemies.push_back(e);
											}
										}
									}
									else {
										chestIsActive = true;
										enemies.clear();
									}
								}
								else {
									player.ResetFull();
									player.ClearInventory();
									player.setPosition(Config::PLAYER_START_X, Config::PLAYER_START_Y);
									currentLevel = 1;
								}

								StillMenu = false;
								menu.SetSaveMenuOpen(false);
							}
						}
					}
				}
				
				else if (chest.MouseOnChest(mousePosWorld)) {
					sf::Vector2f WhereP = player.getPosition();
					if (std::abs(WhereP.x - chest.GetPositionChest()) <= 150 && chestIsActive) {
						chest.ChestOpen();
						SpawnLevel1(enemies, rm);
						chestIsActive = false;
						if (currentLevel == 1) {
							player.addCoins(100);
							
							Item sword, potion;
							sword.Spawn("Sword", rm.textures["Sword"], chest.GetPositionChest() - 40.f, Config::GROUND_Y - 50.f);
							potion.Spawn("Potion", rm.textures["Potion"], chest.GetPositionChest() + 40.f, Config::GROUND_Y - 50.f);
							droppedItems.push_back(sword);
							droppedItems.push_back(potion);
						}
						else if (currentLevel >= 2) {
							
							Item apple;
							apple.Spawn("Apple", rm.textures["Apple"], chest.GetPositionChest(), Config::GROUND_Y - 50.f);
							droppedItems.push_back(apple);
						}
					}
				}
				else {

					if (!player.IsInventoryOpen() && !player.GetIsDragging()) {

						std::string activeItem = player.GetActiveItem();

						 if (activeItem == "Sword") {
							player.Attack(activeItem); 

							
							sf::FloatRect attackBox;
							sf::Vector2f pPos = player.getPosition();

							
							if (player.isFacingRight()) {
							
								attackBox = sf::FloatRect(pPos.x, pPos.y - 140.f, 140.f, 140.f);
							}
							else {
								
								attackBox = sf::FloatRect(pPos.x - 140.f, pPos.y - 140.f, 140.f, 140.f);
							}

							for (auto& enemy : enemies) {
								if (enemy.IsAlive() && attackBox.intersects(enemy.getBounds())) {
									enemy.TakeDamage(15, pPos.x);
									if (!enemy.IsAlive()) {
										
										rm.sounds["take_coin"].play();
										Coin.Spawn("Coin", rm.textures["Coin"], enemy.CurrentX() + 50.f, enemy.CurrentY() - 50.f);
										droppedItems.push_back(Coin);
										if (rand() % 10 == 0) {
											Item Apple;
											Apple.Spawn("Apple", rm.textures["Apple"], enemy.CurrentX() - rand() % 50 + 10, enemy.CurrentY() - 50.f);
											droppedItems.push_back(Apple);
										}
										if (rand() % 15 == 0) {
											Item Potion;
											Potion.Spawn("Potion", rm.textures["Potion"], enemy.CurrentX() + rand() % 50 + 10, enemy.CurrentY() - 50.f);
											droppedItems.push_back(Potion);
										}

									}
								}
							}
						}
						 else if (activeItem == "Potion") {
							 player.UsePotion();
						 }
					}
				}
			}

			if (event.type == sf::Event::MouseButtonReleased) {
				player.CheckDragEnd(mousePosUI);
			}

			if (StillMenu) {
				if (event.type == sf::Event::KeyReleased) {
					if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) menu.MoveUp();
					if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) menu.MoveDown();
					if (event.key.code == sf::Keyboard::Enter) {
						if (menu.GetIndex() == 0) {

							if (menu.GetPaused()) {
								StillMenu = false;
								menu.SetPaused(false);
							}
							else {
								menu.SetSaveMenuOpen(true);
								std::vector<SaveSlotInfo> slots = dbManager.GetSaveSlots();
								menu.UpdateSaveSlots(slots);
							}
						}

						if (menu.GetIndex() == 1) {
							window.close();
						}
					}
				}
			}
			if (event.type == sf::Event::MouseMoved) {
				menu.UpdateMouse(mousePosUI);
			}
		}

		if (StillMenu) {
			window.setView(window.getDefaultView());
			window.draw(spback);
			menu.draw(window);
		}
		else if (inShop) {
			window.setView(window.getDefaultView());
			window.draw(fallbackBg);
			if (shopSize.x > 0) window.draw(shopBg);

		
			if (currentShopTab == 0) {
				btnTabShop.setFillColor(sf::Color(139, 0, 0));
				btnTabUpgrade.setFillColor(sf::Color(80, 80, 80));
			}
			else {
				btnTabShop.setFillColor(sf::Color(80, 80, 80));
				btnTabUpgrade.setFillColor(sf::Color(139, 0, 0));
			}

			window.draw(btnTabShop);
			window.draw(textTabShop);
			window.draw(btnTabUpgrade);
			window.draw(textTabUpgrade);
			window.draw(shopPanel);


			if (currentShopTab == 0) {

			}
			else if (currentShopTab == 1) {
				
				const auto& inv = player.getInventory();
				float startX = shopPanel.getPosition().x + 20.f;
				float startY = shopPanel.getPosition().y + 20.f;
				int itemIndex = 0;

				sf::RectangleShape itemRow(sf::Vector2f(960.f, 80.f));
				itemRow.setFillColor(sf::Color(40, 40, 40));
				itemRow.setOutlineThickness(2.f);
				itemRow.setOutlineColor(sf::Color(100, 100, 100));

				sf::Sprite icon;
				sf::Text nameText("", rm.font, 24);
				sf::Text costText("", rm.font, 24);

				for (const auto& pair : inv) {
					std::string itemName = pair.first;
					InventoryItem itemData = pair.second;

					if (itemData.isUpgradable) {
						itemRow.setPosition(startX, startY + (itemIndex * 90.f));
						window.draw(itemRow);

						icon.setTexture(rm.textures[itemName]);
						float scaleX = 60.f / icon.getTexture()->getSize().x;
						float scaleY = 60.f / icon.getTexture()->getSize().y;
						icon.setScale(scaleX, scaleY);
						icon.setPosition(itemRow.getPosition().x + 10.f, itemRow.getPosition().y + 10.f);
						window.draw(icon);

						nameText.setString(itemName + " (Lv." + std::to_string(itemData.level) + ")");
						nameText.setPosition(icon.getPosition().x + 80.f, icon.getPosition().y + 15.f);
						nameText.setFillColor(sf::Color::White);
						window.draw(nameText);

						costText.setPosition(itemRow.getPosition().x + 650.f, icon.getPosition().y + 15.f);

						if (itemData.level >= 5) {
							costText.setString("MAX LEVEL");
							costText.setFillColor(sf::Color::Red);
						}
						else {
							int upgradeCost = (itemData.level + 1) * 50;
							costText.setString("Upgrade: " + std::to_string(upgradeCost) + " Coins");

							if (player.getCoins() >= upgradeCost) costText.setFillColor(sf::Color::Green);
							else costText.setFillColor(sf::Color(255, 100, 100));
						}
						window.draw(costText);
						itemIndex++;
					}
				}
			}

			window.draw(btnNext);
			window.draw(btnNextText);
			player.drawHUD(window, true);
		}
		
		else {
			if (!isFading) {
				player.Update(dt); 
				
				float px = player.getPosition().x;

				

				if (px >= 4300.f && !wave2Spawned) {
					SpawnWave2(enemies, rm);
					wave2Spawned = true; 
					SpawnWave1(enemies, rm);
					wave1Spawned = true;
				}	

					if (player.GetDeathAlpha() >= 255.f) {
						
						

						gameOverTimer += dt;
						if (gameOverTimer >= 3.0f) {
							bgMusic.stop();
							bgMusic.play();
							player.ResetFull();
							enemies.clear();
							axes.clear();
							currentLevel = 1;
							droppedItems.clear();
							chest.SetPosition(400.f);
							chestIsActive = true;
							StillMenu = true;
							gameOverTimer = 0.f;
							wave2Spawned = false;
							wave1Spawned = false;
						}
					}
					if (!StillMenu) {
						for (auto& enemy : enemies) {
							if (enemy.IsAlive() || enemy.GetDying()) {
								enemy.Update(dt, player.getPosition());
								
								if (enemy.CanThrow()) {
									float startX = enemy.CurrentX();
									float startY = enemy.CurrentY() - 40.f; 
									float targetX = player.getPosition().x;

									
									float speedX = 400.f;
									float vx = (targetX > startX) ? speedX : -speedX;

									
									float distance = std::abs(targetX - startX);
									float t = distance / speedX;

									
									float vy = -(Config::GRAVITY * t) / 2.0f;

									
									axes.emplace_back(rm.textures["scythe"], startX, startY, vx, vy);
								}
								if (!player.IsDead()) {
									if (enemy.IsAlive() && !enemy.GetDying() && player.getBounds().intersects(enemy.getBounds())) {
										player.TakeDamage(enemy.GetDamage(), enemy.CurrentX());
									}
								}
							}
						}
					}
				
				if (player.GetHealth() <= 0) {
					player.Die();
				}
			}
			for (auto& item : droppedItems) {
				if (item.IsActive()) {
					item.Update(dt);
					
						if (player.getBounds().intersects(item.getBounds())) {
							if (item.getName() == "Coin") {
								
								player.addCoins(rand() % 30 + 10);
							}
							
							else {
								player.addItem(item.getName(), 1);
							}
							item.PickUp();
						
						}
					
					
				}
			}
			for (auto& axe : axes) {
				if (axe.isActive) {
					axe.Update(dt);

					
					if (!player.IsDead() && axe.sprite.getGlobalBounds().intersects(player.getBounds())) {
						player.TakeDamage(25, axe.sprite.getPosition().x); 
						axe.isActive = false;
					}
				}
			}

			if (player.getBounds().intersects(portal.getGlobalBounds()) && !isFading) {
				isFading = true;
			}

			if (isFading && !player.IsDead()) {
				fadeAlpha += 120.f * dt;
				if (fadeAlpha >= 255.f) {
					fadeAlpha = 0.f;
					inShop = true;
					isFading = false;
					droppedItems.clear();
				}
				fadeScreen.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)fadeAlpha));
			}

			
			sf::Vector2f pos = player.getPosition();
			float halfView = window.getSize().x / 2.f;
			if (pos.x <= halfView) view.setCenter(halfView, view.getCenter().y);
			else if (pos.x > Config::MAP_WIDTH - halfView) view.setCenter(Config::MAP_WIDTH - halfView, view.getCenter().y);
			else { if (!player.IsDead()) view.setCenter(pos.x, view.getCenter().y); }

			window.setView(view);
			window.draw(spback);
			window.draw(portal);
			chest.DrawChest(window);

			for (auto& item : droppedItems) item.Draw(window);
			
			for (auto& enemy : enemies) {
				if ((enemy.IsAlive() || enemy.GetDying()) && enemy.CurrentY() <= player.getPosition().y) {
					enemy.Draw(window);
				}
			}

			for (auto& axe : axes) {
				if (axe.isActive) window.draw(axe.sprite);
			}
			player.draw(window);

			
			for (auto& enemy : enemies) {
				if ((enemy.IsAlive() || enemy.GetDying()) && enemy.CurrentY() > player.getPosition().y) {
					enemy.Draw(window);
				}
			}
			
			window.setView(window.getDefaultView());

			player.drawHUD(window);
			if (player.GetDeathAlpha() >= 255.f) {
				window.draw(gameOverText);
				
				
			}

			if (fadeAlpha > 0) window.draw(fadeScreen);
		}
		window.display();
	}
}