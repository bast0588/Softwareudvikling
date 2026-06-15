#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include "sqlite3.h"
#include "character.h"

struct CharacterSummary
{
    int id;
    std::string name;
};

class Database
{
public:
    Database(const std::string& path = "game.db");
    ~Database();

    //init
    bool open();
    void createTables();

    //character save and load
    int saveCharacter(const Character& c);
    bool deleteCharacter(int characterId);
    std::vector<CharacterSummary> getAllCharacters();
    Character loadCharacter(int characterId);

    // Stats
    void recordMonsterDefeated(int characterId, const std::string& monsterName);
    void recordItemUsed(int characterId, const std::string& itemName, const std::string& monsterName);

private:
    std::string dbPath;
    sqlite3* db;

    bool exec(const std::string& sql);

    int  getOrCreateItemDef(const Item& item);
    int  getOrCreateMonsterType(const std::string& name);
    Item loadItemDef(int itemDefId);

    StatusType statusTypeFromString(const std::string& s);
    std::string statusTypeToString(StatusType t);
};

#endif // DATABASE_H
