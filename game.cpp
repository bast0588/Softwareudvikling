#include "game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

//helper function
int readIntLine(){
    std::string line;
    getline(std::cin, line);

    //Converts to int
    try{
        return stoi(line);
    } catch (...) {
        std::cout << "Invalid input, try again\n";
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

    itemDatabase.push_back(Item("Bomb", 10, 0.0f,  StatusType::None));
    itemDatabase.push_back(Item("Fire bomb", 5, 0.35f, StatusType::Stunned));
    itemDatabase.push_back(Item("Troll bomb", 10, 0.5f,  StatusType::Paralyzed));
    itemDatabase.push_back(Item("Big stick", 20, 0.0f,  StatusType::None));
    itemDatabase.push_back(Item("Cold metal cube", 0, 0.8f,  StatusType::Frozen));
    itemDatabase.push_back(Item("Cursed scroll", 0, 0.2f,  StatusType::Cursed));  // 100% hvis fjenden har status
    itemDatabase.push_back(Item("2 week old stew", 0, 0.2f,  StatusType::Poisoned)); // 100% hvis
}
void Game::start()
{
    srand(time(nullptr));
    while (true) {
        std::cout << "\n--- MAIN MENU ---\n";
        std::cout << "1. Start new charakter\n";
        std::cout << "2. Quit\n";
        int choice = readIntLine();
        if (choice == 1){
            Character c = createCharacter();
            adventure(c);
        } else if (choice == 2){
            std::cout << "Bye bye \n";
            break;
        } else  {
            std::cout << "Invalid input, try again\n";
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
    if(name == "GOD"){
        cheatAddEveryItem(c);
    }
    return c;
}

void Game::adventure(Character &c)
{
    while (true) {
        std::cout << "\n--- ADVENTURE MENU ---\n";
        std::cout << "1. Fight monster\n";
        std::cout << "2. Enter a cave\n";
        std::cout << "3. View players monster\n";
        std::cout << "4. Inventory / giv item to monster\n";
        std::cout << "5. Heal player monsters\n";
        std::cout << "6. Main menu\n";

        int choice = readIntLine();
        switch (choice) {
        case 1:
            fightMenu(c);
            if (!c.hasAliveMonsters()){
                std::cout << "All your monsters are dead. Return to main menu\n";
                return;
            }
            break;
        case 2:
            dungeonMenu(c);
            if (!c.hasAliveMonsters()){
                std::cout << "All your monsters are dead. Return to main menu\n";
                return;
            }
            break;
        case 3:
            c.printMonster();
            break;
        case 4:
            inventoryMenu(c);
            break;
        case 5:
            healPlayerMonster(c);
            break;
        case 6:
            return;
            break;
        default:
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
    Monster* playerMonster = &c.getNextAliveMonster();
    bool playerTurn = rand() % 2;

    //Main fighting loop
    while (!playerMonster->isDead() && !enemy.isDead()){
        if(playerTurn){ //Spillers tur
            bool skipTurn = false;
            playerMonster->processStatusEffects(skipTurn);
            if(playerMonster->isDead()) {
                break;
            }
            if(!skipTurn){
                playerTurnAction(c, playerMonster, enemy);
            }
        } else {
            // Modstanders tur – status
            bool skipTurn = false;
            enemy.processStatusEffects(skipTurn);
            if (enemy.isDead()) break;
            if (!skipTurn) {
                int dmg = enemy.getStrength();

                // Tjek om fjenden er Cursed – tager selv skade
                bool enemyCursed = false;
                for (auto& s : enemy.getStatuses()) {
                    if (s.getType() == StatusType::Cursed) enemyCursed = true;
                }

                playerMonster->setHP(playerMonster->getHP() - dmg);
                std::cout << "Enemy " << enemy.getName() << " deals " << dmg << " Damage! Player " << playerMonster->getName() << " has " << playerMonster->getHP() << " HP!\n";
                if (enemyCursed) {
                    enemy.setHP(enemy.getHP() - dmg);
                    std::cout << enemy.getName() << " is cursed nad takes " << dmg << " damage!\n";
                    std::cout << enemy.getName() << " has " << enemy.getHP() << " HP\n";
                }
            }
        }
        //Checks if Player monster is dead
        if(playerMonster->isDead()){ //Checks if player monster is dead
            std::cout << playerMonster->getName() << " is dead \n";
            if(!c.hasAliveMonsters()){
                break;
            } else {
                playerMonster = &c.getNextAliveMonster();
                std::cout << "Changing to " << playerMonster->getName() << "\n";
            }
        }
        playerTurn = !playerTurn;
    }
    if (!playerMonster->isDead()){
        std::cout << "You beat " << enemy.getName() << "!\n";
        std::cout << "Adding " << enemy.getName() << " to roster\n";
        c.addMonster(enemy);
    }
}

void Game::playerTurnAction(Character &c, Monster *playerMonster, Monster &enemy)
{
    bool hasItems = !playerMonster->getItems().empty();
    std::cout << "\n" << playerMonster->getName() << "s turn:\n";
    std::cout << "1. attack (" << playerMonster->getStrength() << " damage)\n";
    if (hasItems) std::cout << "2. Use item\n";

    int choice = readIntLine();

    if (choice == 1) {
        int dmg = playerMonster->getStrength();

        bool attackerCursed = false; //Check for cursed
        for (auto& s : playerMonster->getStatuses()) {
            if (s.getType() == StatusType::Cursed) attackerCursed = true;
        }

        enemy.setHP(enemy.getHP() - playerMonster->getStrength());
        std::cout << "Player " << playerMonster->getName() << " Deals " << playerMonster->getStrength() << " Damage! Enemy " << enemy.getName() << " has " << enemy.getHP() << " HP!\n";

        if (attackerCursed) {
            playerMonster->setHP(playerMonster->getHP() - dmg);
            std::cout << playerMonster->getName() << " is cursed nad takes " << dmg <<" damage\n";
            std::cout << playerMonster->getName() << " has " << playerMonster->getHP() << " HP\n";
        }
    } else if (choice == 2 && hasItems) {
        useItemInFight(playerMonster, enemy);
    } else {
        std::cout << "Invalid input – Attacks automatic\n";
        enemy.setHP(enemy.getHP() - playerMonster->getStrength());
    }
}

static float resolveItemChance(const Item& item, const Monster& target, bool targetTookDamage)
{
    float chance = item.getStatusChance();
    if (item.getStatusType() == StatusType::Cursed) {
        // 100% If enemy allready have a status
        if (!const_cast<Monster&>(target).getStatuses().empty())
            chance = 1.0f;
        else
            chance = 0.2f;
    } else if (item.getStatusType() == StatusType::Poisoned) {
        // 100% if enemy hp != 100%
        if (targetTookDamage)
            chance = 1.0f;
        else
            chance = 0.2f;
    }
    return chance;
}

void Game::useItemInFight(Monster *playerMonster, Monster &enemy)
{
    const std::vector<Item>& items = playerMonster->getItems();
    if (items.empty()) {
        std::cout << playerMonster->getName() << " has no items!\n";
        return;
    }
    std::cout << "Chose an item:\n";
    for (int i = 0; i < (int)items.size(); i++) {
        std::cout << i << ": " << items[i].getName() << " (DMG: " << items[i].getDamage() << ")\n";
    }
    int idx = readIntLine();
    if (idx < 0 || idx >= (int)items.size()) {
        std::cout << "Invalid input\n";
        return;
    }

    const Item& chosenItem = items[idx];

    // Damage
    if (chosenItem.getDamage() > 0) {
        enemy.setHP(enemy.getHP() - chosenItem.getDamage());
        std::cout << playerMonster->getName() << " use " << chosenItem.getName() << " and deals " << chosenItem.getDamage() << " damage! Enemy HP: " << enemy.getHP() << "\n";
    }

    // Status
    if (chosenItem.getStatusType() != StatusType::None) {
        bool tookDamage = (enemy.getHP() < 999);
        float chance = resolveItemChance(chosenItem, enemy, tookDamage);
        if ((float)(rand() % 100) / 100.0f < chance) {
            int duration = 2;
            int dpt = (chosenItem.getStatusType() == StatusType::Poisoned) ? 2 : 0;
            enemy.addStatus(Status(chosenItem.getStatusType(), duration, dpt));
            std::cout << "Enemy status: ";
            switch (chosenItem.getStatusType()) {
            case StatusType::Stunned:
                std::cout << "Stunned\n";
                break;
            case StatusType::Paralyzed:
                std::cout << "Paralyzed\n";
                break;
            case StatusType::Frozen:
                std::cout << "Frozen\n";
                break;
            case StatusType::Cursed:
                std::cout << "Cursed\n";
                break;
            case StatusType::Poisoned:
                std::cout << "Poisoned\n";
                break;
            default: break;
            }
        } else {
            std::cout << "Status failed!\n";
        }
    }
}

Dungeon Game::generateDungeon(const Character &player)
{
    Dungeon d;
    int avgHP  = player.averageMonsterHP();
    int avgStr = player.averageMonsterStrength();

    int numMonsters = 2 + rand() % 3; //number of monster = 2-4
    std::vector<int> candidates;
    for (int i = 0; i < (int)monsterDatabase.size(); i++) {
        // Generates a Dungeon monster database
        if (monsterDatabase[i].getHP() <= avgHP * 2 + 5 && monsterDatabase[i].getStrength() <= avgStr * 2 + 3) {
            candidates.push_back(i);
        }
    }
    if (candidates.empty()) {
        // Fallback: To weekest monsters
        candidates.push_back(0);
        candidates.push_back(1);
    }

    //Monsters gets added in a random order
    for (int i = 0; i < numMonsters; i++) {
        int idx = candidates[rand() % candidates.size()];
        d.addMonster(monsterDatabase[idx]);
    }

    // Reward generator
    int rewardIdx = rand() % itemDatabase.size();
    d.setReward(itemDatabase[rewardIdx]);
    return d;
}

void Game::dungeonMenu(Character &c)
{
    std::cout << "\n--- cave menu ---\n";
    Dungeon d = generateDungeon(c);

    std::cout << "You look at a cave with " << d.getMonsters().size() << " monstre:\n";
    for (const auto& m : d.getMonsters()) {
        std::cout << "  - " << m.getName() << " (HP: " << m.getHP() << ", STR: " << m.getStrength() << ")\n";
    }
    std::cout << "Reward: " << d.getReward().getName() << "\n";
    std::cout << "Would you like to enter?\n1. Yes\n2. No\n";
    int choice = readIntLine();
    if (choice != 1) {
        std::cout << "You leave the area\n";
        return;
    }
    fightDungeon(c, d);
}

void Game::fightDungeon(Character &c, Dungeon &d)
{
    const std::vector<Monster>& monsters =d.getMonsters();
    std::vector<Monster> enemies(monsters.begin(),monsters.end());

    std::cout << "You are now entering the dungeon\n";

    for (int i =0; i < enemies.size(); i++){
        if(!c.hasAliveMonsters()){
            std::cout << "All your monsters are dead! You run\n";
            return;
        }
        std::cout << "\n[Dungeon fight " << (i+1) << "/" << enemies.size() << "] against" << enemies[i].getName() << "\n";
        fight(c, enemies[i]);
        if(!c.hasAliveMonsters()){
            std::cout << "All your monsters are dead! You run\n";
            return;
        }
    }

    //Victory
    std::cout << "\n YOU WON THE DUNGEON!\n";
    std::cout << "You find " << d.getReward().getName() << "\n";
    c.addItemToInventory(d.getReward());
    std::cout << d.getReward().getName() << " has been added to invintory";
}

void Game::inventoryMenu(Character &c)
{
    const std::vector<Item>& inv = c.getInventory();
    if (inv.empty()){
        std::cout << "Your inventrory is empty. Fight a dungeon to get an item\n";
        return;
    }

    std::cout << "\n--- INVENTORY ---\n";
    for (int i = 0; i < (int)inv.size(); i++) {
        std::cout << i << ": " << inv[i].getName() << " (DMG: " << inv[i].getDamage() << ") Status type " << "\n";
    }

    std::cout << "Give item [INPUT NUMBER] to a monster. Else input -1\n";
    int itemIdx = readIntLine();
    if (itemIdx < 0 || itemIdx >= (int)inv.size()) {
        std::cout << "Returning to adventure menu\n";
        return;
    }

    std::cout << "Chose a monster to give " << inv[itemIdx].getName() << "\n";
    c.printMonster();
    std::cout << "Enter number: ";
    int monIdx = readIntLine();

    c.givItemToMonster(monIdx,itemIdx);
}

void Game::cheatAddEveryItem(Character& c){
    for(auto& i : itemDatabase){
        c.addItemToInventory(i);
    }
}

void Game::healPlayerMonster(Character& c){
    c.printMonster();
    std::cout << "Healing monsters\n";
    c.healMonsters();
    c.printMonster();
    std::cout << "Monsters have been healed to max HP\n";
}
