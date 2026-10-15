#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <SFML/Audio.hpp>

class ResourceManager {
public:
    std::map<std::string, sf::Texture> textures;
    sf::Font font;

    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::map<std::string, sf::Sound> sounds;

    void LoadAll() {
        textures["idle"].loadFromFile("heroidle.png");
        textures["run"].loadFromFile("hero.png");
        textures["jump"].loadFromFile("jumpTexture.png");
        textures["Apple"].loadFromFile("apple.png");
        textures["Sword"].loadFromFile("sword.png");
        textures["Potion"].loadFromFile("potion.png");
        textures["chest"].loadFromFile("chest.png");
        textures["background"].loadFromFile("background.jpg");
        textures["portal"].loadFromFile("teleport.png");
        textures["shop_bg"].loadFromFile("shop_bg.jpg");
        textures["zombie"].loadFromFile("zombie.png");
        textures["slash_Sword"].loadFromFile("slash_Sword.png");
        textures["zombie_hurt"].loadFromFile("zombie_hurt.png");
        textures["zombie_death"].loadFromFile("zombie_death.png");
        textures["blood"].loadFromFile("blood.png");
        textures["Coin"].loadFromFile("coin.png");
        textures["orc"].loadFromFile("orc.png");
        textures["orc_die"].loadFromFile("orc_die.png");
        textures["orc_hurt"].loadFromFile("orc_hurt.png");
        textures["mini"].loadFromFile("mini.png");
        textures["mini_die"].loadFromFile("mini_die.png");
        textures["mini_hurt"].loadFromFile("mini_hurt.png");
        textures["wiking"].loadFromFile("wiking.png");
        textures["wiking_die"].loadFromFile("wiking_die.png");
        textures["wiking_hurt"].loadFromFile("wiking_hurt.png");
        textures["wiking_throw"].loadFromFile("wiking_throw.png");
        textures["scythe"].loadFromFile("scythe.png");

        soundBuffers["throw"].loadFromFile("throw.ogg");
        sounds["throw"].setBuffer(soundBuffers["throw"]);
        sounds["throw"].setVolume(50.f);
        soundBuffers["death"].loadFromFile("death.ogg");
        sounds["death"].setBuffer(soundBuffers["death"]);
        sounds["death"].setVolume(50.f);
        soundBuffers["monster_die"].loadFromFile("monster_die.ogg");
        sounds["monster_die"].setBuffer(soundBuffers["monster_die"]);
        sounds["monster_die"].setVolume(30.f);
        soundBuffers["take_coin"].loadFromFile("take_coin.ogg");
        sounds["take_coin"].setBuffer(soundBuffers["take_coin"]);
        sounds["take_coin"].setVolume(30.f);
        soundBuffers["drink"].loadFromFile("drink.ogg");
        sounds["drink"].setBuffer(soundBuffers["drink"]);
        sounds["drink"].setVolume(30.f);
        soundBuffers["menu"].loadFromFile("menu.ogg");
        sounds["menu"].setBuffer(soundBuffers["menu"]);
        sounds["menu"].setVolume(30.f);
        soundBuffers["hit"].loadFromFile("hit.ogg");
        sounds["hit"].setBuffer(soundBuffers["hit"]);
        sounds["hit"].setVolume(20.f);
        soundBuffers["vzmah"].loadFromFile("vzmah.ogg");
        sounds["vzmah"].setBuffer(soundBuffers["vzmah"]);
        sounds["vzmah"].setVolume(20.f);
        soundBuffers["hitgg"].loadFromFile("hitgg.ogg");
        sounds["hitgg"].setBuffer(soundBuffers["hitgg"]);
        sounds["hitgg"].setVolume(20.f);
        soundBuffers["walk"].loadFromFile("walk.ogg");
        sounds["walk"].setBuffer(soundBuffers["walk"]);
        sounds["walk"].setVolume(20.f);
        soundBuffers["jump"].loadFromFile("jump.ogg");
        sounds["jump"].setBuffer(soundBuffers["jump"]);
        sounds["jump"].setVolume(20.f);

        font.loadFromFile("font.ttf");
    }
};