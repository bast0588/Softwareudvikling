#include "game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

// Helper: read an int from a full line
int readIntLine(){
    std::string line;
    getline(std::cin, line);
    try {
        return stoi(line);
    } catch (...) {
        std::cout << "Invalid input, try again\n";
        return readIntLine();
    }
}

// Constructor
Game::Game() : db("game.db")
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

    itemDatabase.push_back(Item("Bomb",              10, 0.0f,  StatusType::None));
    itemDatabase.push_back(Item("Fire bomb",          5, 0.35f, StatusType::Stunned));
    itemDatabase.push_back(Item("Troll bomb",        10, 0.5f,  StatusType::Paralyzed));
    itemDatabase.push_back(Item("Big stick",         20, 0.0f,  StatusType::None));
    itemDatabase.push_back(Item("Cold metal cube",    0, 0.8f,  StatusType::Frozen));
    itemDatabase.push_back(Item("Cursed scroll",      0, 0.2f,  StatusType::Cursed));
    itemDatabase.push_back(Item("2 week old stew",    0, 0.2f,  StatusType::Poisoned));
}

// Start
void Game::start()
{
    srand(time(nullptr));
    db.open();
    mainMenu();
}

void Game::mainMenu()
{
    while (true) {
        std::cout << "\n=== MAIN MENU ===\n";
        std::cout << "1. New character\n";
        std::cout << "2. Load character\n";
        std::cout << "3. Delete character\n";
        std::cout << "4. Quit\n";
        int choice = readIntLine();
        switch (choice) {
        case 1: {
            Character c = createCharacter();
            adventure(c);
            break;
        }
        case 2: {
            auto saves = db.getAllCharacters();
            if (saves.empty()) {
                std::cout << "No saved characters found.\n";
                break;
            }
            std::cout << "\n--- Saved characters ---\n";
            for (auto& cs : saves) {
                std::cout << cs.id << ": " << cs.name << "\n";
            }
            std::cout << "Enter character id (or -1 to cancel): ";
            int id = readIntLine();
            if (id < 0) break;
            try {
                Character c = db.loadCharacter(id);
                c.setDbId(id);
                std::cout << "Loaded '" << c.getName() << "'!\n";
                adventure(c);
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << "\n";
            }
            break;
        }
        case 3:
            deleteMenu();
            break;
        case 4:
            std::cout << "Bye bye!\n";
            return;
        default:
            std::cout << "Invalid input\n";
        }
    }
}

// Karakter
Character Game::createCharacter()
{
    std::cout << "Name your character: ";
    std::string name;
    getline(std::cin, name);
    Character c(name);
    c.addMonster(Monster("Hest", 4, 1));
    c.addMonster(Monster("Hest", 4, 1));
    if (name == "GOD") cheatAddEveryItem(c);
    return c;
}

// Adventure loop
void Game::adventure(Character &c)
{
    while (true) {
        std::cout << "\n--- ADVENTURE MENU --- [" << c.getName() << "]\n";
        std::cout << "1. Fight monster\n";
        std::cout << "2. Enter a cave\n";
        std::cout << "3. View monsters\n";
        std::cout << "4. Inventory / give item to monster\n";
        std::cout << "5. Heal monsters\n";
        std::cout << "6. Save character\n";
        std::cout << "7. Main menu\n";

        int choice = readIntLine();
        switch (choice) {
        case 1:
            fightMenu(c);
            if (!c.hasAliveMonsters()) {
                std::cout << "All your monsters are dead. Returning to main menu.\n";
                return;
            }
            break;
        case 2:
            dungeonMenu(c);
            if (!c.hasAliveMonsters()) {
                std::cout << "All your monsters are dead. Returning to main menu.\n";
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
            saveMenu(c);
            break;
        case 7:
            return;
        default:
            std::cout << "Invalid input\n";
        }
    }
}

// Kamp
void Game::fightMenu(Character &c)
{
    std::cout << "Choose a monster to fight:\n";
    for (int i = 0; i < (int)monsterDatabase.size(); i++) {
        std::cout << i << ": " << monsterDatabase[i].getName() << " (HP: " << monsterDatabase[i].getHP() << ", STR: " << monsterDatabase[i].getStrength() << ")\n";
    }
    int idx = readIntLine();
    if (idx < 0 || idx >= (int)monsterDatabase.size()) {
        std::cout << "Invalid choice\n";
        return;
    }
    Monster enemy = monsterDatabase[idx];
    fight(c, enemy);
}

void Game::fight(Character &c, Monster enemy)
{
    std::cout << "Starting fight against " << enemy.getName() << "!\n";
    Monster* playerMonster = &c.getNextAliveMonster();
    bool playerTurn = rand() % 2;

    while (!playerMonster->isDead() && !enemy.isDead()) {
        if (playerTurn) {
            bool skipTurn = false;
            playerMonster->processStatusEffects(skipTurn);
            if (playerMonster->isDead()) break;
            if (!skipTurn) playerTurnAction(c, playerMonster, enemy);
        } else {
            bool skipTurn = false;
            enemy.processStatusEffects(skipTurn);
            if (enemy.isDead()) break;
            if (!skipTurn) {
                int dmg = enemy.getStrength();
                bool enemyCursed = false;
                for (auto& s : enemy.getStatuses())
                    if (s.getType() == StatusType::Cursed) enemyCursed = true;

                playerMonster->setHP(playerMonster->getHP() - dmg);
                std::cout << enemy.getName() << " deals " << dmg << " damage! " << playerMonster->getName() << " has " << playerMonster->getHP() << " HP\n";

                if (enemyCursed) {
                    enemy.setHP(enemy.getHP() - dmg);
                    std::cout << enemy.getName() << " is cursed and takes " << dmg << " damage! HP: " << enemy.getHP() << "\n";
                }
            }
        }

        if (playerMonster->isDead()) {
            std::cout << playerMonster->getName() << " is dead!\n";
            if (!c.hasAliveMonsters()) break;
            playerMonster = &c.getNextAliveMonster();
            std::cout << "Switching to " << playerMonster->getName() << "\n";
        }
        playerTurn = !playerTurn;
    }

    if (!playerMonster->isDead()) {
        std::cout << "You beat " << enemy.getName() << "!\n";
        // Statistik
        if (c.getDbId() != -1)
            db.recordMonsterDefeated(c.getDbId(), enemy.getName());
        c.addMonster(enemy);
    }
}

void Game::playerTurnAction(Character &c, Monster *playerMonster, Monster &enemy)
{
    bool hasItems = !playerMonster->getItems().empty();
    std::cout << "\n" << playerMonster->getName() << "'s turn:\n";
    std::cout << "1. Attack (" << playerMonster->getStrength() << " damage)\n";
    if (hasItems) std::cout << "2. Use item\n";

    int choice = readIntLine();

    if (choice == 1) {
        int dmg = playerMonster->getStrength();
        bool attackerCursed = false;
        for (auto& s : playerMonster->getStatuses())
            if (s.getType() == StatusType::Cursed) attackerCursed = true;

        enemy.setHP(enemy.getHP() - dmg);
        std::cout << playerMonster->getName() << " deals " << dmg << " damage! "
                  << enemy.getName() << " has " << enemy.getHP() << " HP\n";

        if (attackerCursed) {
            playerMonster->setHP(playerMonster->getHP() - dmg);
            std::cout << playerMonster->getName() << " is cursed and takes " << dmg << " damage! HP: " << playerMonster->getHP() << "\n";
        }
    } else if (choice == 2 && hasItems) {
        useItemInFight(playerMonster, enemy, c);
    } else {
        std::cout << "Invalid input – attacking automatically\n";
        enemy.setHP(enemy.getHP() - playerMonster->getStrength());
    }
}

static float resolveItemChance(const Item& item, const Monster& target, bool targetTookDamage)
{
    float chance = item.getStatusChance();
    if (item.getStatusType() == StatusType::Cursed) {
        chance = const_cast<Monster&>(target).getStatuses().empty() ? 0.2f : 1.0f;
    } else if (item.getStatusType() == StatusType::Poisoned) {
        chance = targetTookDamage ? 1.0f : 0.2f;
    }
    return chance;
}

void Game::useItemInFight(Monster *playerMonster, Monster &enemy, Character &c)
{
    const std::vector<Item>& items = playerMonster->getItems();
    if (items.empty()) { std::cout << "No items!\n"; return; }

    std::cout << "Choose an item:\n";
    for (int i = 0; i < (int)items.size(); i++) {
        std::cout << i << ": " << items[i].getName() << " (DMG: " << items[i].getDamage() << ")\n";
    }
    int idx = readIntLine();
    if (idx < 0 || idx >= (int)items.size()) { std::cout << "Invalid input\n"; return; }

    const Item& chosen = items[idx];

    // Statistik
    if (c.getDbId() != -1)
        db.recordItemUsed(c.getDbId(), chosen.getName(), playerMonster->getName());

    if (chosen.getDamage() > 0) {
        enemy.setHP(enemy.getHP() - chosen.getDamage());
        std::cout << playerMonster->getName() << " uses " << chosen.getName() << " and deals " << chosen.getDamage() << " damage! Enemy HP: " << enemy.getHP() << "\n";
    }

    if (chosen.getStatusType() != StatusType::None) {
        bool tookDamage = (enemy.getHP() < 999);
        float chance = resolveItemChance(chosen, enemy, tookDamage);
        if ((float)(rand() % 100) / 100.0f < chance) {
            int duration = 2;
            int dpt = (chosen.getStatusType() == StatusType::Poisoned) ? 2 : 0;
            enemy.addStatus(Status(chosen.getStatusType(), duration, dpt));
            std::cout << "Enemy got status effect!\n";
        } else {
            std::cout << "Status effect failed!\n";
        }
    }
}

// Dungeon
Dungeon Game::generateDungeon(const Character &player)
{
    Dungeon d;
    int avgHP  = player.averageMonsterHP();
    int avgStr = player.averageMonsterStrength();
    int numMonsters = 2 + rand() % 3;

    std::vector<int> candidates;
    for (int i = 0; i < (int)monsterDatabase.size(); i++) {
        if (monsterDatabase[i].getHP() <= avgHP  * 2 + 5 && monsterDatabase[i].getStrength() <= avgStr * 2 + 3)
            candidates.push_back(i);
    }
    if (candidates.empty()) { candidates.push_back(0); candidates.push_back(1); }

    for (int i = 0; i < numMonsters; i++)
        d.addMonster(monsterDatabase[candidates[rand() % candidates.size()]]);

    d.setReward(itemDatabase[rand() % itemDatabase.size()]);
    return d;
}

void Game::dungeonMenu(Character &c)
{
    std::cout << "\n--- CAVE MENU ---\n";
    Dungeon d = generateDungeon(c);

    std::cout << "You see a cave with " << d.getMonsters().size() << " monsters:\n";
    for (const auto& m : d.getMonsters())
        std::cout << "  - " << m.getName() << " (HP: " << m.getHP() << ", STR: " << m.getStrength() << ")\n";
    std::cout << "Reward: " << d.getReward().getName() << "\n";
    std::cout << "Enter the cave?\n1. Yes\n2. No\n";

    if (readIntLine() != 1) { std::cout << "You back away.\n"; return; }
    fightDungeon(c, d);
}

void Game::fightDungeon(Character &c, Dungeon &d)
{
    std::vector<Monster> enemies(d.getMonsters().begin(), d.getMonsters().end());
    std::cout << "Entering the dungeon!\n";

    for (int i = 0; i < (int)enemies.size(); i++) {
        if (!c.hasAliveMonsters()) { std::cout << "All monsters dead – you flee!\n"; return; }
        std::cout << "\n[Fight " << (i+1) << "/" << enemies.size() << "] vs " << enemies[i].getName() << "\n";
        fight(c, enemies[i]);
        if (!c.hasAliveMonsters()) { std::cout << "All monsters dead – you flee!\n"; return; }
    }

    std::cout << "\nYOU CLEARED THE DUNGEON!\n";
    std::cout << "You found: " << d.getReward().getName() << "\n";
    c.addItemToInventory(d.getReward());
    std::cout << d.getReward().getName() << " added to inventory.\n";
}

// Inventory
void Game::inventoryMenu(Character &c)
{
    const std::vector<Item>& inv = c.getInventory();
    if (inv.empty()) { std::cout << "Inventory is empty. Clear a dungeon to get items.\n"; return; }

    std::cout << "\n--- INVENTORY ---\n";
    for (int i = 0; i < (int)inv.size(); i++)
        std::cout << i << ": " << inv[i].getName() << " (DMG: " << inv[i].getDamage() << ")\n";

    std::cout << "Give item [number] to a monster, or -1 to cancel: ";
    int itemIdx = readIntLine();
    if (itemIdx < 0 || itemIdx >= (int)inv.size()) { std::cout << "Cancelled.\n"; return; }

    std::cout << "Choose a monster:\n";
    c.printMonster();
    int monIdx = readIntLine();
    c.givItemToMonster(monIdx, itemIdx);
}

// Database menus
void Game::saveMenu(Character &c)
{
    std::cout << "Saving '" << c.getName() << "'...\n";
    int id = db.saveCharacter(c);
    c.setDbId(id);
    std::cout << "Saved! (id=" << id << ")\n";
}

void Game::deleteMenu()
{
    auto saves = db.getAllCharacters();
    if (saves.empty()) { std::cout << "No saved characters.\n"; return; }

    std::cout << "\n--- DELETE CHARACTER ---\n";
    for (auto& cs : saves)
        std::cout << cs.id << ": " << cs.name << "\n";
    std::cout << "Enter id to delete (or -1 to cancel): ";
    int id = readIntLine();
    if (id < 0) { std::cout << "Cancelled.\n"; return; }
    db.deleteCharacter(id);
}

// Helpers
void Game::cheatAddEveryItem(Character &c)
{
    for (auto& i : itemDatabase) c.addItemToInventory(i);
}

void Game::healPlayerMonster(Character &c)
{
    c.healMonsters();
    std::cout << "Monsters healed to max HP.\n";
    c.printMonster();
}
