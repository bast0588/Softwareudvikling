#ifndef DUNGEON_H
#define DUNGEON_H

#include <vector>
#include "monster.h"
#include "item.h"

class Dungeon
{
public:
    Dungeon();
    void addMonster(const Monster& m);
    void setReward(const Item& item);
    const Item& getReward() const;
    const std::vector<Monster>& getMonsters() const;
private:
    std::vector<Monster> monsters;
    Item reward;
};

#endif // DUNGEON_H
