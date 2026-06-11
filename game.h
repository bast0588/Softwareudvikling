#ifndef GAME_H
#define GAME_H

#include <vector>
#include "character.h"
#include "monster.h"

class Game{
public:
    Game();
    void start();
private:
    Character createCharacter();
    void adventure(Character& c);
    void fightMenu(Character& c);
    void fight(Character& c, Monster enemy);
    std::vector<Monster> monsterDatabase;
};

#endif // GAME_H
