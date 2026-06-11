#include "game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

//helper function
int readIntLine(){
    std::string line;
    getline(std::cin, line);

    //Converts to int
    try{
        return stoi(line);
    } catch (...) {
        std::cout << "Invalid input, try again";
        return readIntLine();
    }
}
Game::Game()
{
    monsterDatabase.push_back(Monster("Hest", 4, 1));
    monsterDatabase.push_back(Monster("Weak Goblin", 4, 2));
    monsterDatabase.push_back(Monster("Strong Goblin", 8, 3));
    monsterDatabase.push_back(Monster("Stronger Goblin", 10, 4));
    monsterDatabase.push_back(Monster("Den stærkeste Goblin", 15, 5));
    monsterDatabase.push_back(Monster("Abe Kongen", 30, 5));
    monsterDatabase.push_back(Monster("Enhjørning", 50, 8));
    monsterDatabase.push_back(Monster("Drage", 100, 10));
    monsterDatabase.push_back(Monster("Skarpt Papir", 1, 15));
}

void Game::start()
{
    srand(time(nullptr));
    while (true) {
        std::cout << "\n--- MAIN MENU ---\n";
        std::cout << "1. Start new charakter\n";
        std::cout << "2. Quit\n";
        int choice = readIntLine();
        std::cout << "You have choicen " << choice;
        if (choice == 1){
            Character c = createCharacter();
            adventure(c);
        } else if (choice == 2){
            std::cout << "Bye bye \n";
            break;
        } else  {
            std::cout << "invalidt input";
        }
    }
}

Character Game::createCharacter()
{
    std::cout << "Name your character: ";
    std::string name;
    getline(std::cin, name);
    Character c(name);
    c.addMonster(Monster("Hest", 4, 1));
    c.addMonster(Monster("Hest", 4, 1));
    return c;
}

void Game::adventure(Character &c)
{
    while (true) {
        std::cout << "\n--- ADVENTURE MENU ---\n";
        std::cout << "1. Fight monster\n";
        std::cout << "2. Main menu\n";

        int choice = readIntLine();

        if (choice == 1){
            fightMenu(c);
            if (!c.hasAliveMonsters()){
                std::cout << "All your monsters are dead. Return to main menu\n";
                return;
            }
        } else if(choice == 2){
            return;
        } else {
            std::cout << "invalid input\n";
        }
    }
}

void Game::fightMenu(Character &c)
{
    std::cout << "Chose a monster to fight\n";
    for (int i=0; i < monsterDatabase.size(); i++){
        std::cout << i << ": " << monsterDatabase[i].getName() << " (HP: " << monsterDatabase[i].getHP() << ", STR: " << monsterDatabase[i].getStrength() << ")\n";
    }
    int idx = readIntLine();
    if (idx < 0 || idx >= monsterDatabase.size()){
        std::cout << "Invalid answer\n";
        return;
    }
    Monster enemy = monsterDatabase[idx];
    fight(c, enemy);
}

void Game::fight(Character &c, Monster enemy)
{
    std::cout << "Staring fight against " << enemy.getName() <<"!\n";
    Monster& playerMonster = c.getNextAliveMonster();
    bool playerTurn = rand() % 2;
    while (!playerMonster.isDead() && !enemy.isDead()){ //Main fighting loop
        if(playerTurn){ //Spillers tur
            enemy.setHP(enemy.getHP() - playerMonster.getStrength());
            std::cout << playerMonster.getName() << "Deals " << playerMonster.getStrength() << "Damage! " << enemy.getName() << " has " << enemy.getHP() << " HP!\n";
        } else { // Modstanders tur
            playerMonster.setHP(playerMonster.getHP() - enemy.getStrength());
            std::cout << enemy.getName() << "Deals " << enemy.getStrength() << "Damage! " << playerMonster.getName() << " has " << playerMonster.getHP() << " HP!\n";
        }
        playerTurn = !playerTurn;
    }
    if (playerMonster.isDead()){
        std::cout << playerMonster.getName() << "died!\n";
    } else {
        std::cout << "You beat " << enemy.getName() << "!\n";
        c.addMonster(enemy);
    }
}
