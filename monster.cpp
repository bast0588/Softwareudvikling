#include "monster.h"

monster::monster(){

}

monster::monster(std::string navn, int HP, int styrke)
{
    this -> hp = HP;
    this -> styrke = styrke;
    this -> navn = navn;
}

void monster::setHp(int nyHp)
{
    this -> hp = nyHp;
}

int monster::getHp()
{
    return hp;
}

int monster::getStyrke()
{
    return styrke;
}

void monster::setStyrke(int nyStyrke)
{
    this -> styrke = nyStyrke;
}

std::string monster::getNavn()
{
    return navn;
}
