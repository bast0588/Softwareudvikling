#include <QCoreApplication>
#include "karakter.h"
#include "monster.h"
#include <iostream>
int main(){
    karakter human("John");
    monster hest("Hest", 4, 1);
    human.changeMonsterRoster(hest,0);
    human.getMonster(0).setHp(2);
    std::cout << "test\n";
    std::cout << hest.getHp();
    std::cout << human.getMonster(0).getHp();
}
