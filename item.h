#ifndef ITEM_H
#define ITEM_H

class Item
{
public:
    Item(std::string n, int dmg, float chance, StatusType st);
    std::string name;
    int damage;
    float statusChance;
    StatusType statusToApply;
};

#endif // ITEM_H
