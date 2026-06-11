#ifndef MONSTER_H
#define MONSTER_H
#include <string>

class monster {
public:
    monster();
    monster(std::string navn, int HP, int styrke);
    void setHp(int nyHp);
    int getHp();
    int getStyrke();
    void setStyrke(int nyStyrke);
    std::string getNavn();

private:
    std::string navn;
    int hp;
    int styrke;
};

#endif // MONSTER_H
