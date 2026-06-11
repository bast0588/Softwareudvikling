#include "monster.h"

Monster::Monster(std::string name, int hp, int strength)
    : name(name), hp(hp), strength(strength){
}

bool Monster::isDead() const{
    return hp <= 0;
}

void Monster::setHP(int nyHp)
{
    this -> hp = nyHp;
}

int Monster::getHP()
{
    return hp;
}

int Monster::getStrength()
{
    return strength;
}

void Monster::setStrength(int newStrength)
{
    this -> strength = newStrength;
}

std::string Monster::getName()
{
    return name;
}
