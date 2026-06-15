cmake_minimum_required(VERSION 3.10)
project(Softwareudvikling LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Softwareudvikling
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

target_link_libraries(Softwareudvikling PRIVATE dl pthread)