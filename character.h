#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <vector>
#include "monster.h"

class Character{
public:
    Character(std::string name);

    std::string getName();
    void setName(std::string newName);

    void addMonster(const Monster& m);
    bool hasAliveMonsters() const;
    Monster& getNextAliveMonster();
    void printMonster();
    int averageMonsterHP() const;
    int averageMonsterStrength() const;
    void healMonsters();
    //Item
    void addItemToInventory(const Item& item);
    void givItemToMonster(int monsterIndex, int itemIndex);
    const std::vector<Item>& getInventory() const;

private:
    std::string name;
    std::vector<Monster> monsters;
    std::vector<Item> itemInventory;
};

#endif // CHARACTER_H
