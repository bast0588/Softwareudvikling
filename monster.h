#ifndef MONSTER_H
#define MONSTER_H
#include <string>
#include <vector>
#include "status.h"
#include "item.h"

class Monster {
public:
    Monster(const std::string name, int HP, int stength);

    bool isDead() const;
    void setHP(int newHP);
    int getHP() const;
    void heal();
    int getStrength() const;
    void setStrength(int newStrength);
    std::string getName() const;

    //Items
    void addItem(const Item& item);
    const std::vector<Item>& getItems() const;
    std::string printItems();
    bool hasItem() const;
    //status
    void addStatus(const Status& status);
    std::vector<Status>& getStatuses();
    void processStatusEffects(bool& skipTurn);

private:
    std::string name;
    int hp;
    int maxHP;
    int strength;
    std::vector<Item> items;
    std::vector<Status> statuses;
};

#endif // MONSTER_H
