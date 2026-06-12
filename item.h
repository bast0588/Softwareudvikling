#ifndef ITEM_H
#define ITEM_H

#include <string>
#include "status.h"

class Item
{
public:
    Item(const std::string& name, int damage, float statusChance, StatusType statusType);
    std::string getName() const;
    int getDamage() const;
    float getStatusChance() const;
    StatusType getStatusType() const;

private:
    std::string name;
    int damage;
    float statusChance;
    StatusType statusToApply;
};

#endif // ITEM_H
