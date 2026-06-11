#include "karakter.h"
#include "monster.h"


karakter::karakter(std::string navn)
{
    this -> navn = navn;
}

monster karakter::getMonster(int i)
{
    if (i > sizeof(monstre) or i <0){
        return monstre[0]; //Placeholder skal ændres til en fejlbesked senere
    }
    return monstre[i];
}

std::string karakter::getNavn()
{
    return navn;
}

void karakter::setNavn(std::string nytNavn)
{
    this -> navn = nytNavn;
}

void karakter::changeMonsterRoster(monster nytMonster, int i)
{
    monstre[i] = nytMonster;
}
