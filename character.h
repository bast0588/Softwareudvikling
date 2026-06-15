#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <vector>
#include "monster.h"

class Character{
public:
    Character(std::string name);

    std::string getName() const;
    void setName(std::string newName);

    //DB
    int getDbId() const;
    void setDbId(int id);

    void addMonster(const Monster& m);
    bool hasAliveMonsters() const;
    Monster& getNextAliveMonster();
    void printMonster();
    int averageMonsterHP() const;
    int averageMonsterStrength() const;
    void healMonsters();
    const std::vector<Monster>& getMonstersConst() const;

    //Item
    void addItemToInventory(const Item& item);
    void givItemToMonster(int monsterIndex, int itemIndex);
    const std::vector<Item>& getInventory() const;

private:
    std::string name;
    int dbId = -1;
    std::vector<Monster> monsters;
    std::vector<Item> itemInventory;
};

#endif // CHARACTER_H
