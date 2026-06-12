#include "dungeon.h"

Dungeon::Dungeon()
    : reward("No Reward", 0, 0.0f, StatusType::None) {}

void Dungeon::addMonster(const Monster &m)
{
    monsters.push_back(m);
}

void Dungeon::setReward(const Item &item)
{
    reward = item;
}

const Item &Dungeon::getReward() const
{
    return reward;
}

const std::vector<Monster> &Dungeon::getMonsters() const
{
    return monsters;
}
