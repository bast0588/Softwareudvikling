#include "character.h"
#include <iostream>
#include <stdexcept>

Character::Character(std::string name)
    : name(name){
}

std::string Character::getName()
{
    return name;
}

void Character::addMonster(const Monster &m)
{
    if(monsters.size() < 4){
        monsters.push_back(m);
    } else {
        std::cout << "You allready have 4 monsters. Switch one? (y/n): ";
        char c;
        std::cin >> c;
        if(c == 'y'){
            std::cout << "What monster should kick the bin?\n";
            for(int i = 0; i < monsters.size(); i++){
                std::cout << i << ": " << monsters[i].getName() << "\n";
            }
            int idx; // værdig til at vælge monsteret der skal udskiftes
            std::cin >> idx;
            if(idx >= 0 && idx < monsters.size()){
                monsters[idx] = m;
            }
        }
    }
}

bool Character::hasAliveMonsters() const
{
    for(const auto& m : monsters){
        if(!m.isDead()){
            return true;
        }
    }
    return false;
}

Monster &Character::getNextAliveMonster()
{
    for(auto& m : monsters){
        if(!m.isDead()){
            return m;
        }
    }
    throw std::runtime_error("No living mosters!");
}

void Character::printMonster()
{
    int i = 0;

    for(auto& m : monsters){
        std::cout << i <<": " << m.getName() << " (HP: " << m.getHP() << ", STR: " << m.getStrength() << ") \n";
        if(m.hasItem()){
            std::cout << "Itmes in inventory " << m.printItems() <<"\n";
        }
        i++;
    }
}

int Character::averageMonsterHP() const
{
    if(monsters.empty()){
        return 0;
    }
    int sum = 0;
    for (const auto& m : monsters){
        sum += m.getHP();
    }
    return sum / monsters.size();
}

int Character::averageMonsterStrength() const
{
    if(monsters.empty()){
        return 0;
    }
    int sum = 0;
    for (const auto& m : monsters){
        sum += m.getStrength();
    }
    return sum / monsters.size();
}

void Character::healMonsters()
{
    for(auto&m : monsters) {
        m.heal();
    }
}

void Character::addItemToInventory(const Item &item)
{
    itemInventory.push_back(item);
}

void Character::givItemToMonster(int monsterIndex, int itemIndex)
{
    if (monsterIndex < 0 || monsterIndex >= monsters.size()) {
        std::cout << "Invalid monster index\n";
        return;
    }
    if (itemIndex < 0 || itemIndex >= itemInventory.size()) {
        std::cout << "Invalid item index\n";
        return;
    }
    monsters[monsterIndex].addItem(itemInventory[itemIndex]);
    std::cout << itemInventory[itemIndex].getName() << " Given to " << monsters[monsterIndex].getName() << "\n";

    itemInventory.erase(itemInventory.begin()+itemIndex);

}

const std::vector<Item> &Character::getInventory() const
{
    return itemInventory;
}

