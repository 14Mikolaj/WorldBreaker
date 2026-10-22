cmake_minimum_required(VERSION 3.16)
project(WorldBreaker LANGUAGES CXX)

if(NOT WIN32 OR NOT MINGW)
    message(FATAL_ERROR
        "World Breaker requires Windows with a MinGW-w64 C++ compiler. "
        "The original game uses Windows console APIs and commands; "
        "native Linux/macOS builds are not supported.")
endif()

set(GAME_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Project files")

add_executable(WorldBreaker
    "${GAME_SOURCE_DIR}/main.cpp"
    "${GAME_SOURCE_DIR}/src/Character.cpp"
    "${GAME_SOURCE_DIR}/src/Enemy.cpp"
    "${GAME_SOURCE_DIR}/src/Item.cpp"
    "${GAME_SOURCE_DIR}/src/Menus.cpp"
    "${GAME_SOURCE_DIR}/src/Player.cpp"
)
target_include_directories(WorldBreaker PRIVATE "${GAME_SOURCE_DIR}/include")
set_target_properties(WorldBreaker PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/game"
)
target_compile_options(WorldBreaker PRIVATE -Wall -Wextra)

# The game looks up lib/ relative to its working directory. Copy only game
# resources, never the saved characters or other files from the old workspace.
set(GAME_DATA_DIR "${CMAKE_CURRENT_BINARY_DIR}/game/lib")
file(MAKE_DIRECTORY "${GAME_DATA_DIR}/items" "${GAME_DATA_DIR}/debug"
    "${GAME_DATA_DIR}/save")
file(COPY "${GAME_SOURCE_DIR}/lib/locations" DESTINATION "${GAME_DATA_DIR}")
file(COPY "${GAME_SOURCE_DIR}/lib/save/default" DESTINATION "${GAME_DATA_DIR}/save")
configure_file("${GAME_SOURCE_DIR}/lib/items/item_list.il"
    "${GAME_DATA_DIR}/items/item_list.il" COPYONLY)
configure_file("${GAME_SOURCE_DIR}/lib/debug/debug_location.txt"
    "${GAME_DATA_DIR}/debug/debug_location.txt" COPYONLY)

# Reconfiguring or rebuilding must not erase an existing player's progress.
foreach(SLOT RANGE 1 3)
    set(SLOT_DIR "${GAME_DATA_DIR}/save/${SLOT}")
    file(MAKE_DIRECTORY "${SLOT_DIR}")
    if(NOT EXISTS "${SLOT_DIR}/player.sav")
        file(WRITE "${SLOT_DIR}/player.sav" "NEW CHARACTER\n")
    endif()
    foreach(EMPTY_FILE inv.inv skills.skill)
        if(NOT EXISTS "${SLOT_DIR}/${EMPTY_FILE}")
            file(WRITE "${SLOT_DIR}/${EMPTY_FILE}" "")
        endif()
    endforeach()
endforeach()
if(NOT EXISTS "${GAME_DATA_DIR}/save/graveyard.txt")
    file(WRITE "${GAME_DATA_DIR}/save/graveyard.txt" "")
endif()
