#include "database.h"
#include <iostream>
#include <stdexcept>

Database::Database(const std::string& path) : dbPath(path), db(nullptr) {}

Database::~Database() {
    if (db) sqlite3_close(db);
}

bool Database::open() {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Could not open database: " << sqlite3_errmsg(db) << "\n";
        return false;
    }
    exec("PRAGMA foreign_keys = ON;");
    createTables();
    return true;
}

bool Database::exec(const std::string& sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// Skema
void Database::createTables() {

    // --- Lookup-tabeller (normaliserede definitioner) ---

    // Alle unikke item-typer med deres stats.
    // Fixes 3NF: items and character_inventory previously duplicated this data.
    exec(R"(
        CREATE TABLE IF NOT EXISTS item_definitions (
            id            INTEGER PRIMARY KEY AUTOINCREMENT,
            name          TEXT    NOT NULL UNIQUE,
            damage        INTEGER NOT NULL,
            status_chance REAL    NOT NULL,
            status_type   TEXT    NOT NULL
        );
    )");

    exec(R"(
        CREATE TABLE IF NOT EXISTS monster_types (
            id   INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE
        );
    )");

    // --- Kerne-tabeller ---

    exec(R"(
        CREATE TABLE IF NOT EXISTS characters (
            id   INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL
        );
    )");

    // A monster belonging to a character. HP and strength are stored here because
    // they represent the character's current state (may differ from monster_types).
    exec(R"(
        CREATE TABLE IF NOT EXISTS monsters (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            character_id    INTEGER NOT NULL,
            monster_type_id INTEGER NOT NULL,
            hp              INTEGER NOT NULL,
            strength        INTEGER NOT NULL,
            FOREIGN KEY(character_id)    REFERENCES characters(id),
            FOREIGN KEY(monster_type_id) REFERENCES monster_types(id)
        );
    )");

    // Items on a monster – FK to item_definitions only, no duplicated stats.
    exec(R"(
        CREATE TABLE IF NOT EXISTS monster_items (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            monster_id  INTEGER NOT NULL,
            item_def_id INTEGER NOT NULL,
            FOREIGN KEY(monster_id)  REFERENCES monsters(id),
            FOREIGN KEY(item_def_id) REFERENCES item_definitions(id)
        );
    )");

    // Items in the character's inventory – FK to item_definitions only.
    exec(R"(
        CREATE TABLE IF NOT EXISTS character_inventory (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            character_id INTEGER NOT NULL,
            item_def_id  INTEGER NOT NULL,
            FOREIGN KEY(character_id) REFERENCES characters(id),
            FOREIGN KEY(item_def_id)  REFERENCES item_definitions(id)
        );
    )");

    // --- Statistik-tabeller ---

    exec(R"(
        CREATE TABLE IF NOT EXISTS monsters_defeated (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            character_id    INTEGER NOT NULL,
            monster_type_id INTEGER NOT NULL,
            FOREIGN KEY(character_id)    REFERENCES characters(id),
            FOREIGN KEY(monster_type_id) REFERENCES monster_types(id)
        );
    )");

    exec(R"(
        CREATE TABLE IF NOT EXISTS items_used (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            character_id    INTEGER NOT NULL,
            item_def_id     INTEGER NOT NULL,
            monster_type_id INTEGER NOT NULL,
            FOREIGN KEY(character_id)    REFERENCES characters(id),
            FOREIGN KEY(item_def_id)     REFERENCES item_definitions(id),
            FOREIGN KEY(monster_type_id) REFERENCES monster_types(id)
        );
    )");
}

// Normalization helpers

// Returns the id of the item_definition with this name.
// Creates the row if it does not already exist.
int Database::getOrCreateItemDef(const Item& item) {
    sqlite3_stmt* stmt;

    // Try to find existing
    std::string findSql = "SELECT id FROM item_definitions WHERE name = ?;";
    sqlite3_prepare_v2(db, findSql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, item.getName().c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return id;
    }
    sqlite3_finalize(stmt);

    // Opret ny
    std::string insSql = "INSERT INTO item_definitions (name, damage, status_chance, status_type) VALUES (?,?,?,?);";
    sqlite3_prepare_v2(db, insSql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, item.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, item.getDamage());
    sqlite3_bind_double(stmt, 3, item.getStatusChance());
    sqlite3_bind_text(stmt, 4, statusTypeToString(item.getStatusType()).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (int)sqlite3_last_insert_rowid(db);
}

// Returns the id of the monster_type with this name.
// Creates the row if it does not already exist.
int Database::getOrCreateMonsterType(const std::string& name) {
    sqlite3_stmt* stmt;

    std::string findSql = "SELECT id FROM monster_types WHERE name = ?;";
    sqlite3_prepare_v2(db, findSql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return id;
    }
    sqlite3_finalize(stmt);

    std::string insSql = "INSERT INTO monster_types (name) VALUES (?);";
    sqlite3_prepare_v2(db, insSql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (int)sqlite3_last_insert_rowid(db);
}

// Loads an Item object from an item_def id.
Item Database::loadItemDef(int itemDefId) {
    sqlite3_stmt* stmt;
    std::string sql = "SELECT name, damage, status_chance, status_type FROM item_definitions WHERE id = ?;";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, itemDefId);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("item_definition not found: " + std::to_string(itemDefId));
    }
    std::string name   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    int         dmg    = sqlite3_column_int(stmt, 1);
    float       chance = (float)sqlite3_column_double(stmt, 2);
    std::string stype  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    sqlite3_finalize(stmt);
    return Item(name, dmg, chance, statusTypeFromString(stype));
}

// StatusType
std::string Database::statusTypeToString(StatusType t) {
    switch (t) {
    case StatusType::Stunned: return "Stunned";
    case StatusType::Paralyzed: return "Paralyzed";
    case StatusType::Frozen: return "Frozen";
    case StatusType::Cursed: return "Cursed";
    case StatusType::Poisoned: return "Poisoned";
    default: return "None";
    }
}

StatusType Database::statusTypeFromString(const std::string& s) {
    if (s == "Stunned") return StatusType::Stunned;
    if (s == "Paralyzed") return StatusType::Paralyzed;
    if (s == "Frozen") return StatusType::Frozen;
    if (s == "Cursed") return StatusType::Cursed;
    if (s == "Poisoned") return StatusType::Poisoned;
    return StatusType::None;
}

// Save
int Database::saveCharacter(const Character& c) {
    // Slet eksisterende karakter med samme navn
    sqlite3_stmt* stmt;
    int existingId = -1;
    std::string findSql = "SELECT id FROM characters WHERE name = ?;";
    sqlite3_prepare_v2(db, findSql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, c.getName().c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) existingId = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    if (existingId != -1) deleteCharacter(existingId);

    // Insert character
    std::string charSql = "INSERT INTO characters (name) VALUES (?);";
    sqlite3_prepare_v2(db, charSql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, c.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    int charId = (int)sqlite3_last_insert_rowid(db);

    // Gem monstre
    for (const Monster& m : c.getMonstersConst()) {
        int typeId = getOrCreateMonsterType(m.getName());

        std::string msql = "INSERT INTO monsters (character_id, monster_type_id, hp, strength) VALUES (?,?,?,?);";
        sqlite3_prepare_v2(db, msql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, charId);
        sqlite3_bind_int(stmt, 2, typeId);
        sqlite3_bind_int(stmt, 3, m.getHP());
        sqlite3_bind_int(stmt, 4, m.getStrength());
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        int monsterId = (int)sqlite3_last_insert_rowid(db);

        // Save items on the monster – FK only
        for (const Item& item : m.getItems()) {
            int defId = getOrCreateItemDef(item);
            std::string isql = "INSERT INTO monster_items (monster_id, item_def_id) VALUES (?,?);";
            sqlite3_prepare_v2(db, isql.c_str(), -1, &stmt, nullptr);
            sqlite3_bind_int(stmt, 1, monsterId);
            sqlite3_bind_int(stmt, 2, defId);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
    }

    // Gem karakter inventory – kun FK
    for (const Item& item : c.getInventory()) {
        int defId = getOrCreateItemDef(item);
        std::string isql = "INSERT INTO character_inventory (character_id, item_def_id) VALUES (?,?);";
        sqlite3_prepare_v2(db, isql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, charId);
        sqlite3_bind_int(stmt, 2, defId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    std::cout << "Character '" << c.getName() << "' saved (id=" << charId << ")\n";
    return charId;
}

// Delete
bool Database::deleteCharacter(int characterId) {
    std::string cid = std::to_string(characterId);
    exec("DELETE FROM monster_items WHERE monster_id IN "
         "(SELECT id FROM monsters WHERE character_id = " + cid + ");");
    exec("DELETE FROM monsters          WHERE character_id = " + cid + ";");
    exec("DELETE FROM character_inventory WHERE character_id = " + cid + ";");
    exec("DELETE FROM monsters_defeated  WHERE character_id = " + cid + ";");
    exec("DELETE FROM items_used         WHERE character_id = " + cid + ";");
    exec("DELETE FROM characters         WHERE id = "           + cid + ";");
    std::cout << "Character deleted.\n";
    return true;
}

// Load
std::vector<CharacterSummary> Database::getAllCharacters() {
    std::vector<CharacterSummary> result;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, name FROM characters ORDER BY id;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return result;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        CharacterSummary cs;
        cs.id   = sqlite3_column_int(stmt, 0);
        cs.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        result.push_back(cs);
    }
    sqlite3_finalize(stmt);
    return result;
}

Character Database::loadCharacter(int characterId) {
    sqlite3_stmt* stmt;

    // Hent karakternavn
    std::string charSql = "SELECT name FROM characters WHERE id = ?;";
    sqlite3_prepare_v2(db, charSql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, characterId);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Character not found in database");
    }
    std::string charName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    sqlite3_finalize(stmt);

    Character c(charName);

    // Fetch monsters – JOIN with monster_types to get the name
    std::string msql = R"(
        SELECT m.id, mt.name, m.hp, m.strength
        FROM monsters m
        JOIN monster_types mt ON mt.id = m.monster_type_id
        WHERE m.character_id = ?;
    )";
    sqlite3_prepare_v2(db, msql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, characterId);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int         mId   = sqlite3_column_int(stmt, 0);
        std::string mName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int         mHp   = sqlite3_column_int(stmt, 2);
        int         mStr  = sqlite3_column_int(stmt, 3);

        Monster m(mName, mHp, mStr);

        // Fetch items on the monster via JOIN with item_definitions
        sqlite3_stmt* istmt;
        std::string isql = R"(
            SELECT id.id
            FROM monster_items mi
            JOIN item_definitions id ON id.id = mi.item_def_id
            WHERE mi.monster_id = ?;
        )";
        sqlite3_prepare_v2(db, isql.c_str(), -1, &istmt, nullptr);
        sqlite3_bind_int(istmt, 1, mId);
        while (sqlite3_step(istmt) == SQLITE_ROW) {
            int defId = sqlite3_column_int(istmt, 0);
            m.addItem(loadItemDef(defId));
        }
        sqlite3_finalize(istmt);

        c.addMonster(m);
    }
    sqlite3_finalize(stmt);

    // Hent karakter inventory via JOIN med item_definitions
    std::string invsql = R"(
        SELECT id.id
        FROM character_inventory ci
        JOIN item_definitions id ON id.id = ci.item_def_id
        WHERE ci.character_id = ?;
    )";
    sqlite3_prepare_v2(db, invsql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, characterId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int defId = sqlite3_column_int(stmt, 0);
        c.addItemToInventory(loadItemDef(defId));
    }
    sqlite3_finalize(stmt);

    return c;
}

// Statistik
void Database::recordMonsterDefeated(int characterId, const std::string& monsterName) {
    int typeId = getOrCreateMonsterType(monsterName);
    sqlite3_stmt* stmt;
    std::string sql = "INSERT INTO monsters_defeated (character_id, monster_type_id) VALUES (?,?);";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, characterId);
    sqlite3_bind_int(stmt, 2, typeId);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Database::recordItemUsed(int characterId, const std::string& itemName, const std::string& usedByMonsterName) {
    // item_def_id is looked up from item_definitions by name
    sqlite3_stmt* stmt;
    int itemDefId = -1;
    std::string findSql = "SELECT id FROM item_definitions WHERE name = ?;";
    sqlite3_prepare_v2(db, findSql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, itemName.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) itemDefId = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    if (itemDefId == -1) return; // Item ikke registreret endnu – spring over

    int typeId = getOrCreateMonsterType(usedByMonsterName);

    std::string sql = "INSERT INTO items_used (character_id, item_def_id, monster_type_id) VALUES (?,?,?);";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, characterId);
    sqlite3_bind_int(stmt, 2, itemDefId);
    sqlite3_bind_int(stmt, 3, typeId);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}
