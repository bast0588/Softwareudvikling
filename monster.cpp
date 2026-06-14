#include "monster.h"
#include <iostream>
#include <algorithm>

Monster::Monster(std::string name, int hp, int strength)
    : name(name), hp(hp), strength(strength), maxHP(hp){
}

bool Monster::isDead() const{
    return hp <= 0;
}

void Monster::setHP(int nyHp)
{
    this -> hp = nyHp;
}

int Monster::getHP() const
{
    return hp;
}

void Monster::heal()
{
    hp = maxHP;
}

int Monster::getStrength() const
{
    return strength;
}

void Monster::setStrength(int newStrength)
{
    this -> strength = newStrength;
}

std::string Monster::getName() const
{
    return name;
}

void Monster::addItem(const Item &item)
{
    items.push_back(item);
    std::cout << "Item resived\n"; //Tester for fejlcode
}

const std::vector<Item> &Monster::getItems() const
{
    return items;
}

std::string Monster::printItems()
{
    std::string itemList = "";
    for(auto& i : items){
        itemList.append(i.getName() + " ");
    }
    return itemList;
}

bool Monster::hasItem() const
{
    return !items.empty();
}

void Monster::addStatus(const Status &status)
{
    statuses.push_back(status);
}

std::vector<Status> &Monster::getStatuses()
{
    return statuses;
}

void Monster::processStatusEffects(bool &skipTurn)
{
    skipTurn = false;
    for (auto& s : statuses) {
        switch (s.getType()) {
        case StatusType::Poisoned: //Poisoned – Takes small damage each turn.
            hp -= s.getDamagePerTurn();
            std::cout << name << " takes " << s.getDamagePerTurn() << " poison damage! HP: " << hp << "\n";
            break;
        case StatusType::Stunned: //Stunned – Guaranteed to miss next turn.
            std::cout << name << " is stunned and loses its turn!\n";
            skipTurn = true;
            break;
        case StatusType::Frozen: //Frozen – Cannot act for 1–2 turns.
            // fx 80% chance for at miste tur
            if (rand() % 100 < 80) {
                std::cout << name << " is frozen and cannot move!\n";
                skipTurn = true;
            }
            break;
        case StatusType::Paralyzed: //Paralyzed – 30% chance to lose turn.
            if (rand() % 100 < 30) {
                std::cout << name << " is paralyzed and cannot move!\n";
                skipTurn = true;
            }
            break;
        case StatusType::Cursed: //Takes damage when dealing damage
            std::cout << name << " is cursed!\n";
            break;
        default:
            break;
        }
        s.decreaseDuration();
    }
    statuses.erase(std::remove_if(statuses.begin(), statuses.end(), [](const Status& s){ return s.getDuration() <= 0; }),statuses.end());
}
