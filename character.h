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

private:
    std::string name;
    std::vector<Monster> monsters;
};

#endif // CHARACTER_H
