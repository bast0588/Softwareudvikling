#ifndef GAME_H
#define GAME_H

#include <vector>
#include "character.h"
#include "monster.h"
#include "dungeon.h"
#include "item.h"

class Game{
public:
    Game();
    void start();
private:
    Character createCharacter();
    void adventure(Character& c);

    // Enkelt kamp
    void fightMenu(Character& c);
    void fight(Character& c, Monster enemy);
    void playerTurnAction(Character& c, Monster* playerMonster, Monster& enemy);
    void useItemInFight(Monster* playerMonster, Monster& enemy);

    //Dungeon
    Dungeon generateDungeon(const Character& player);
    void dungeonMenu(Character& c);
    void fightDungeon(Character& c, Dungeon& d);

    // Inventory
    void inventoryMenu(Character& c);

    std::vector<Monster> monsterDatabase;
    std::vector<Item> itemDatabase;
    void cheatAddEveryItem(Character& c);
    void healPlayerMonster(Character &c);
};

#endif // GAME_H
