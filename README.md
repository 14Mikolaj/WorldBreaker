# World Breaker

[![Build and test](https://github.com/14Mikolaj/WorldBreaker/actions/workflows/build.yml/badge.svg)](https://github.com/14Mikolaj/WorldBreaker/actions/workflows/build.yml)

World Breaker is a text-based fantasy RPG written in C++ for a university
project in 2020. Four classes, an ASCII world map, questionable monsters,
and all the original student-project charm.

Originally built for Windows with Code::Blocks and MinGW, it now also runs
natively on Linux. The world, dialogue, classes, and game systems are the
original ones. Compatibility fixes make it build and play on current systems;
the unmodified source remains in the first commit.

## Download and play

Get the ready-to-run archives from [Releases](https://github.com/14Mikolaj/WorldBreaker/releases/latest).
No compiler or installation is needed.

| Download | Platform | Start the game |
| --- | --- | --- |
| [Windows x64 ZIP](https://github.com/14Mikolaj/WorldBreaker/releases/download/v1.0.0/WorldBreaker-v1.0.0-windows-x64.zip) | 64-bit Windows 10/11 | Extract everything, then double-click `play.bat` |
| [Linux x64 tar.gz](https://github.com/14Mikolaj/WorldBreaker/releases/download/v1.0.0/WorldBreaker-v1.0.0-linux-x64.tar.gz) | x86-64 Linux with glibc 2.35+ (Ubuntu 22.04 or newer, or compatible distributions) | Extract everything, open a terminal in the extracted folder, run `./play.sh` |

Extract into a folder you can write to. Keep the executable and `lib/`
together; the launchers select the correct working directory. Each archive
contains all game data and three empty save slots. SHA-256 checksum files
are included with the release downloads.

The Windows executable includes its MinGW runtimes. The Linux executable
includes its C++ runtime and uses the system's glibc; musl-only systems such
as Alpine are not supported by the prebuilt download.

## Playing

Enter the number beside a menu option, then press Enter. Choose **Start
Game**, select a slot, name your character, and choose a class. Press Enter
when the game pauses between screens.

- **Bruiser**, **Rogue**, **Scholar**, or **Mage**, each with six starting skills.
- Explore connected locations, talk to NPCs, and view the ASCII world map.
- Fight using attacks and skills; agility affects turn order.
- Collect equipment and item drops, gain levels, and allocate stat points.
- Three character slots, with a graveyard for fallen characters.

Use **Save** in the location menu before leaving a session. Saves live in
`lib/save/` inside the extracted game folder. Back up that directory before
replacing or deleting your copy of the game.

## Build from source

Requirements:

- Linux with GCC, or Windows with MinGW-w64 GCC; C++11 support is required.
- CMake 3.16 or newer and Ninja, available on `PATH`.
- Python 3.10+ to run the tests or package a release; not needed to play.
- Git to clone the repository.

From a terminal (PowerShell on Windows):

```sh
git clone https://github.com/14Mikolaj/WorldBreaker.git
cd WorldBreaker
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
cd build/game
```

Then run `./WorldBreaker` on Linux or `.\WorldBreaker.exe` on Windows.
Always launch from `build/game`; the game loads `lib/` relative to its working
directory. CMake prepares the game data and empty slots, normalizes data line
endings, and preserves existing saves when reconfigured.

The original `Project files/WorldBreaker.cbp` is retained as a historical
reference. CMake is the supported build path. Windows builds with MSVC and
macOS builds are not supported.

## Package a release

After building, run from the repository root:

```sh
python scripts/package_release.py --game-dir build/game --platform linux-x64 --version 1.0.0 --output-dir dist
```

For Windows, use `--platform windows-x64` with a Windows x64 build. Packaging
verifies the executable architecture and always creates fresh save slots,
even if the build directory has been used to play. Test the resulting archive:

```sh
python tests/smoke_test.py --archive dist/WorldBreaker-v1.0.0-linux-x64.tar.gz
```

GitHub Actions builds and tests Debug and Release on both platforms, then
packages and tests the Release archives. Official Linux packages are built
on Ubuntu 22.04 to keep the glibc requirement at 2.35.

## Project status

This is a preserved university game, with known rough edges and unfinished
content. Publication fixes cover input-buffer overflows, portable console
handling, Linux data loading, uninitialized state, an escape-from-combat
crash, and equipment loading after a save.

Automated checks cover all four classes, skills, NPC dialogue, map/travel,
inventory save/reload, combat escape, clean input shutdown, and the packaged
launchers. They do not cover a complete playthrough or every combat mechanic.

University reports and declarations, old binaries, object files, IDE state,
duplicate distributions, and personal saves are excluded from the source
repository and release packages.

## License

[MIT](LICENSE). Some names and dialogue reference other games; this student
project is not affiliated with their creators.
