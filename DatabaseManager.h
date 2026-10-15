#pragma once
#include "sqlite3.h"
#include <string>
#include <iostream>
#include <vector>


struct SaveSlotInfo {
    int slotNumber;     
    int level;        
    std::string saveDate;
    bool isEmpty;        
};

struct DBItem {
    std::string name;
    int count;
    int level;
};

class DatabaseManager {
private:
    sqlite3* db;

public:
    DatabaseManager();
    ~DatabaseManager();

    void InitDatabase();
    void SavePlayer(int slot, int hp, float x, float y, int level, int coins);
    std::vector<SaveSlotInfo> GetSaveSlots();
    bool LoadPlayer(int slot, int& hp, float& x, float& y, int& level, int& coins);
    void SaveInventory(int slot, const std::vector<DBItem>& items);
    std::vector<DBItem> LoadInventory(int slot);
    void SaveEntity(int slot, std::string entityId, int state);
    int GetEntityState(int slot, std::string entityId);
    void DeleteSaveSlot(int slot);
    void SaveSettings(int volume, int key);
    bool LoadSettings(int& volume, int& key);
};