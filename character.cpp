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

void Character::setName(std::string newName)
{
    this -> name = newName;
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




