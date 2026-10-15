#include "DatabaseManager.h"

DatabaseManager::DatabaseManager() {
    db = nullptr;
}

DatabaseManager::~DatabaseManager() {
    if (db) {
        sqlite3_close(db);
    }
}

void DatabaseManager::InitDatabase() {

    int rc = sqlite3_open("savegame.db", &db);
    if (rc) {
        std::cout << "Помилка відкриття БД: " << sqlite3_errmsg(db) << "\n";
        return;
    }

    const char* sql = "CREATE TABLE IF NOT EXISTS PlayerData ("
        "SaveSlot INTEGER PRIMARY KEY, "
        "Health INTEGER, "
        "PosX REAL, "
        "PosY REAL, "
        "Level INTEGER, "
        "Coins INTEGER, "
        "SaveDate TEXT);"
       
        "CREATE TABLE IF NOT EXISTS Inventory ("
        "SaveSlot INTEGER, "
        "ItemName TEXT, "
        "Count INTEGER, "
        "Level INTEGER, "
        "PRIMARY KEY(SaveSlot, ItemName));"
        
        "CREATE TABLE IF NOT EXISTS WorldEntities ("
        "SaveSlot INTEGER, "
        "EntityID TEXT, "
        "IsLootedOrDead INTEGER, "
        "PRIMARY KEY(SaveSlot, EntityID));"

        "CREATE TABLE IF NOT EXISTS Settings ("
        "ID INTEGER PRIMARY KEY CHECK (ID = 1), "
        "Volume INTEGER, "
        "PotionKey INTEGER);";


    char* errMsg = nullptr;
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cout << "Помилка SQL: " << errMsg << "\n";
        sqlite3_free(errMsg); 
    }
    else {
        std::cout << "База даних успішно ініціалізована!\n";
    }
}

void DatabaseManager::SavePlayer(int slot, int hp, float x, float y, int level, int coins) {
    if (!db) return; 

    
    std::string sql = "REPLACE INTO PlayerData (SaveSlot, Health, PosX, PosY, Level, Coins,SaveDate) "
        "VALUES (" + std::to_string(slot) + ", " +
        std::to_string(hp) + ", " +
        std::to_string(static_cast<int>(x)) + ", " +
        std::to_string(static_cast<int>(y)) + ", " +
        std::to_string(level) + ", " +
        std::to_string(coins) + ", " +
        "datetime('now', 'localtime'));";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cout << "Помилка збереження: " << errMsg << "\n";
        sqlite3_free(errMsg);
    }
    else {
        std::cout << "Гра успішно збережена у слот " << slot << "!\n";
    }
}

std::vector<SaveSlotInfo> DatabaseManager::GetSaveSlots() {
    
    std::vector<SaveSlotInfo> slots(3);
    for (int i = 0; i < 3; i++) {
        slots[i].slotNumber = i + 1;
        slots[i].isEmpty = true; 
    }

    if (!db) return slots; 

    
    const char* sql = "SELECT SaveSlot, Level, SaveDate FROM PlayerData;";
    sqlite3_stmt* stmt;

    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {

       
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            
            int slotNumber = sqlite3_column_int(stmt, 0);

            
            if (slotNumber >= 1 && slotNumber <= 3) {
                int index = slotNumber - 1;

               
                slots[index].level = sqlite3_column_int(stmt, 1);

                
                const char* dateText = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                slots[index].saveDate = dateText ? dateText : "Unknown time";

                
                slots[index].isEmpty = false;
            }
        }
    }
    
    sqlite3_finalize(stmt);

    return slots;
}

bool DatabaseManager::LoadPlayer(int slot, int& hp, float& x, float& y, int& level, int& coins) {
    if (!db) return false;

    std::string sql = "SELECT Health, PosX, PosY, Level, Coins FROM PlayerData WHERE SaveSlot = " + std::to_string(slot) + ";";
    sqlite3_stmt* stmt;
    bool found = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            hp = sqlite3_column_int(stmt, 0);
            x = static_cast<float>(sqlite3_column_double(stmt, 1));
            y = static_cast<float>(sqlite3_column_double(stmt, 2));
            level = sqlite3_column_int(stmt, 3);
            coins = sqlite3_column_int(stmt, 4);
            found = true;
        }
    }

    sqlite3_finalize(stmt);
    return found;
}

void DatabaseManager::SaveInventory(int slot, const std::vector<DBItem>& items) {
    if (!db) return;

    
    std::string sqlClear = "DELETE FROM Inventory WHERE SaveSlot = " + std::to_string(slot) + ";";
    sqlite3_exec(db, sqlClear.c_str(), nullptr, nullptr, nullptr);

    
    for (const auto& item : items) {
        std::string sqlInsert = "INSERT INTO Inventory (SaveSlot, ItemName, Count, Level) VALUES (" +
            std::to_string(slot) + ", '" +
            item.name + "', " +
            std::to_string(item.count) + ", " +
            std::to_string(item.level) + ");";

        sqlite3_exec(db, sqlInsert.c_str(), nullptr, nullptr, nullptr);
    }
}

std::vector<DBItem> DatabaseManager::LoadInventory(int slot) {
    std::vector<DBItem> items;
    if (!db) return items;

    std::string sql = "SELECT ItemName, Count, Level FROM Inventory WHERE SaveSlot = " + std::to_string(slot) + ";";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            DBItem item;
            item.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            item.count = sqlite3_column_int(stmt, 1);
            item.level = sqlite3_column_int(stmt, 2);
            items.push_back(item);
        }
    }
    sqlite3_finalize(stmt);
    return items;
}
void DatabaseManager::SaveEntity(int slot, std::string entityId, int state) {
    if (!db) return;
  
    std::string sql = "REPLACE INTO WorldEntities (SaveSlot, EntityID, IsLootedOrDead) VALUES (" +
        std::to_string(slot) + ", '" + entityId + "', " + std::to_string(state) + ");";

    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

int DatabaseManager::GetEntityState(int slot, std::string entityId) {
    if (!db) return 0;

    std::string sql = "SELECT IsLootedOrDead FROM WorldEntities WHERE SaveSlot = " +
        std::to_string(slot) + " AND EntityID = '" + entityId + "';";
    sqlite3_stmt* stmt;
    int state = 0;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            state = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return state;
}

void DatabaseManager::DeleteSaveSlot(int slot) {
    if (!db) return;

    std::string slotStr = std::to_string(slot);

    std::string sql =
        "DELETE FROM PlayerData WHERE SaveSlot = " + slotStr + ";"
        "DELETE FROM Inventory WHERE SaveSlot = " + slotStr + ";"
        "DELETE FROM WorldEntities WHERE SaveSlot = " + slotStr + ";";

    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    std::cout << "Слот " << slot << " успішно видалено!\n";
}

void DatabaseManager::SaveSettings(int volume, int key) {
    if (!db) return;
    std::string sql = "INSERT OR REPLACE INTO Settings (ID, Volume, PotionKey) VALUES (1, " +
        std::to_string(volume) + ", " + std::to_string(key) + ");";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

bool DatabaseManager::LoadSettings(int& volume, int& key) {
    if (!db) return false;
    std::string sql = "SELECT Volume, PotionKey FROM Settings WHERE ID = 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            volume = sqlite3_column_int(stmt, 0);
            key = sqlite3_column_int(stmt, 1);
            sqlite3_finalize(stmt);
            return true;
        }
    }
    sqlite3_finalize(stmt);
    return false;
}