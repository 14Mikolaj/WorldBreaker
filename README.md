# World Breaker

[![Windows build](https://github.com/14Mikolaj/WorldBreaker/actions/workflows/build.yml/badge.svg)](https://github.com/14Mikolaj/WorldBreaker/actions/workflows/build.yml)

World Breaker is a text-based fantasy RPG written in C++ for a university
project in 2020. It was originally developed for Windows using Code::Blocks
and MinGW.

This repository contains the original game and data, with a small input-buffer
fix for current compilers. The CMake build and GitHub Actions workflow were
added for publishing the project. The unmodified source is preserved in the
first commit. This is an early learning project with known bugs and unfinished
content, rather than a maintained game release.

## Features

- Four character classes: Bruiser, Rogue, Scholar, and Mage.
- Three save slots, character progression, and stat allocation.
- Connected locations, an ASCII world map, and NPC dialogue.
- Turn-based combat with agility affecting turn order.
- Skills, equipment, inventory, and enemy item drops.
- Character death and a graveyard of fallen characters.

## Requirements

- Windows.
- A MinGW-w64 C++ compiler with C++11 support (`g++`).
- CMake 3.16 or newer and Ninja.
- Git to clone the repository.

The compiler, CMake, and Ninja must be available on `PATH`. The game has no
third-party library dependencies. Native Linux and macOS builds are not
supported: the original code uses `windows.h` and the Windows `CLS`/`PAUSE`
commands. MSVC is not a supported build configuration.

## Build and run

Run these commands in PowerShell with MinGW-w64 available:

```powershell
git clone https://github.com/14Mikolaj/WorldBreaker.git
cd WorldBreaker
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release
cmake --build build
cd build/game
.\WorldBreaker.exe
```

Launch the game from `build/game`, where the build prepares its `lib/` data
directory. Paths are relative to the working directory, not the executable.
CMake initializes three empty character slots and an empty graveyard on the
first configure. Reconfiguring preserves existing saves. After editing game
data, rerun the configure command to refresh the copies in `build/game/lib`.

The original `Project files/WorldBreaker.cbp` project is also included for
reference. The CMake build is the documented way to prepare a fresh runtime
directory without relying on the original saved games.

## Playing

Enter the number shown beside a menu option, then press Enter. Choose
**Start Game**, select a character slot, and follow the prompts to create a
character. In the location menu, you can investigate, talk to NPCs, travel,
view the map, or save. Combat offers attacks, skills, stats, and escape.

Use **Save** before leaving a session. Saves are stored in
`build/game/lib/save/`; keep a copy before removing your build directory.

## Repository contents

| Path | Contents |
| --- | --- |
| `Project files/main.cpp` | Game flow and menus |
| `Project files/include/` | Class and menu declarations |
| `Project files/src/` | Characters, combat, items, and menu implementation |
| `Project files/lib/` | Locations, items, default skills, and fallback location |
| `CMakeLists.txt` | Windows build and fresh runtime setup |

University reports and declarations, compiled binaries, object files, IDE
state, duplicate distributions, and personal saves are excluded from Git.

## Project status

The original menu input buffers could overflow even for a single digit. Those
buffers and character-name input now use strings. Other limitations remain:
some menu variables are uninitialized, and combat escape contains an explicit
destructor call that can cause a crash. The build workflow checks Debug and
Release compilation and opening the character menu, not a complete playthrough.
Further refactoring and gameplay fixes are left for future work.

## License

[MIT](LICENSE). Some names and dialogue reference other games; this student
project is not affiliated with their creators.
