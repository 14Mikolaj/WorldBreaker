cmake_minimum_required(VERSION 3.16)
project(WorldBreaker VERSION 1.0.0 LANGUAGES CXX)

if(WIN32 AND NOT MINGW)
    message(FATAL_ERROR
        "Windows builds currently require a MinGW-w64 C++ compiler.")
elseif(NOT WIN32 AND NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "Supported platforms are Windows and Linux.")
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

option(WORLDBREAKER_STATIC_RUNTIME "Bundle GCC/C++ runtimes in the executable" ON)
if(WORLDBREAKER_STATIC_RUNTIME AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(WorldBreaker PRIVATE -static-libgcc -static-libstdc++)
    if(MINGW)
        # Include winpthreads too, so the Windows ZIP needs only system DLLs.
        target_link_options(WorldBreaker PRIVATE -static)
    endif()
endif()

# The game looks up lib/ relative to its working directory. Copy only game
# resources, never the saved characters or other files from the old workspace.
# Normalize CRLF text: Linux getline otherwise leaves carriage returns in
# location tokens and item/skill names.
set(GAME_DATA_DIR "${CMAKE_CURRENT_BINARY_DIR}/game/lib")
file(GLOB_RECURSE GAME_ASSETS CONFIGURE_DEPENDS RELATIVE "${GAME_SOURCE_DIR}/lib"
    "${GAME_SOURCE_DIR}/lib/locations/*.loc")
list(APPEND GAME_ASSETS
    "locations/_WorldMap.txt"
    "items/item_list.il"
    "debug/debug_location.txt"
    "save/default/skills_bruiser.skill"
    "save/default/skills_rogue.skill"
    "save/default/skills_scholar.skill"
    "save/default/skills_mage.skill")
foreach(ASSET IN LISTS GAME_ASSETS)
    set(ASSET_SOURCE "${GAME_SOURCE_DIR}/lib/${ASSET}")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${ASSET_SOURCE}")
    file(READ "${ASSET_SOURCE}" ASSET_CONTENTS)
    string(REPLACE "\r\n" "\n" ASSET_CONTENTS "${ASSET_CONTENTS}")
    get_filename_component(ASSET_DIRECTORY "${GAME_DATA_DIR}/${ASSET}" DIRECTORY)
    file(MAKE_DIRECTORY "${ASSET_DIRECTORY}")
    file(WRITE "${GAME_DATA_DIR}/${ASSET}" "${ASSET_CONTENTS}")
endforeach()

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

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 3.10 COMPONENTS Interpreter)
    if(Python3_Interpreter_FOUND)
        add_test(NAME gameplay_smoke
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/smoke_test.py"
                --game-dir "$<TARGET_FILE_DIR:WorldBreaker>")
        set_tests_properties(gameplay_smoke PROPERTIES TIMEOUT 120)
    endif()
endif()
