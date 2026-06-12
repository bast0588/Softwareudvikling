cmake_minimum_required(VERSION 3.19)
project(Softwareudvikling LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(Softwareudvikling
    main.cpp
    Monster_list
    monster.h monster.cpp
    character.h character.cpp
    character.cpp
    game.h game.cpp
    item.h item.cpp
    status.h status.cpp
    status.cpp
    dungeon.h dungeon.cpp
)

target_link_libraries(Softwareudvikling
    PRIVATE
        Qt::Core
)

include(GNUInstallDirs)

install(TARGETS Softwareudvikling
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
