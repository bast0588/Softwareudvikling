#include "item.h"

Item::Item(const std::string &name, int damage, float statusChance, StatusType statusType)
    : name(name), damage(damage), statusChance(statusChance), statusType(statusType) {}

std::string Item::getName() const
{
    return name;
}

int Item::getDamage() const
{
    return damage;
}

float Item::getStatusChance() const
{
    return statusChance;
}

StatusType Item::getStatusType() const
{
    return statusType;
}


