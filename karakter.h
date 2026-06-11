#ifndef KARAKTER_H
#define KARAKTER_H
#include <string>
#include "monster.h"
class karakter{

public:
    karakter(std::string navn);
    monster getMonster(int i);
    std::string getNavn();
    void setNavn(std::string nytNavn);
    void changeMonsterRoster(monster nytMonster, int i);

private:
    std::string navn;
    monster monstre[4];
};

#endif // KARAKTER_H
