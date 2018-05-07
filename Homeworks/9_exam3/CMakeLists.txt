
cmake_minimum_required (VERSION 3.0)
set(CMAKE_C_STANDARD 11)

set(TARGET "gol")

set(GAME game.h game.c)
set(PGM pgm_image.h pgm_image.c)
set(STRINGS string_functions.h string_functions.c)

add_executable(${TARGET} game_of_life.c ${GAME} ${PGM} ${STRINGS})