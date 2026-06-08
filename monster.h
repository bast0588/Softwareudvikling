#ifndef MONSTER_H
#define MONSTER_H
#include <string>

class Monster {
public:
    Monster();
    void setHp(int nyHp);
    int getHp();
    int getStyrke();
    void setStyrke(int nyStyrke);
private:
    std::string navn;
    int hp;
    int styrke;
};

#endif // MONSTER_H
