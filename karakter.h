#ifndef KARAKTER_H
#define KARAKTER_H
#include <string>
#include "monster.h"
class karakter{

public:
    karakter();
    Monster getMonster(int i);
    std::string getNavn();
    void setNavn(std::string nytNavn);

private:
    std::string navn;
    Monster monstre[4];
};

#endif // KARAKTER_H
