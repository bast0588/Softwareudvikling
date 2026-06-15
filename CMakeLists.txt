cmake_minimum_required(VERSION 3.19)
project(Softwareudvikling LANGUAGES CXX C)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(Softwareudvikling
    main.cpp
    monster.h monster.cpp
    character.h character.cpp
    game.h game.cpp
    item.h item.cpp
    status.h status.cpp
    dungeon.h dungeon.cpp
    database.h database.cpp
    sqlite3.h sqlite3.c
)

set_source_files_properties(sqlite3.c PROPERTIES LANGUAGE C)
target_compile_definitions(Softwareudvikling PRIVATE SQLITE_OMIT_LOAD_EXTENSION)

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
