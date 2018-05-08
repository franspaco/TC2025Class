
cmake_minimum_required (VERSION 3.0)
set(CMAKE_C_STANDARD 11)

set(SYNC "gol_sync")
set(THREAD "gol_thr")
set(OMP "gol_omp")

set(GAME game.h game.c)
set(PGM pgm_image.h pgm_image.c)
set(STRINGS string_functions.h string_functions.c)
set(PNG tiny_png/TinyPngOut.h tiny_png/TinyPngOut.c)

include(FindOpenMP)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
endif()
add_executable(${SYNC} game_of_life.c ${GAME} ${STRINGS} ${PNG})

add_executable(${THREAD} game_of_life.c ${GAME} ${STRINGS} ${PNG})
target_compile_definitions(${THREAD} PRIVATE SIM_MODE=1)
target_link_libraries(${THREAD} pthread)

add_executable(${OMP} game_of_life.c ${GAME} ${STRINGS} ${PNG})
target_compile_definitions(${OMP} PRIVATE SIM_MODE=2)