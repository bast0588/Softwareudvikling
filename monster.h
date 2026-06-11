#ifndef MONSTER_H
#define MONSTER_H
#include <string>

class Monster {
public:
    Monster(const std::string name, int HP, int stength);

    bool isDead() const;
    void setHP(int newHP);
    int getHP();
    int getStrength();
    void setStrength(int newStrength);
    std::string getName();

private:
    std::string name;
    int hp;
    int strength;
};

#endif // MONSTER_H
