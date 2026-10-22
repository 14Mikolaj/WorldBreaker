#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

// Keep the original Windows console presentation, with a terminal-safe Linux path.
namespace Console {

struct EndOfInput {};

inline bool input_is_terminal() {
#ifdef _WIN32
    return _isatty(_fileno(stdin)) != 0;
#else
    return isatty(fileno(stdin)) != 0;
#endif
}

inline bool output_is_terminal() {
#ifdef _WIN32
    DWORD mode;
    return GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode) != 0;
#else
    const char *term = std::getenv("TERM");
    return isatty(fileno(stdout)) != 0 && term && std::string(term) != "dumb";
#endif
}

inline bool &pending_input_line() {
    static bool pending = false;
    return pending;
}

inline std::string read_token() {
    std::string input;
    if (!(std::cin >> input)) throw EndOfInput();
    pending_input_line() = true;
    return input;
}

inline std::string read_line() {
    std::string input;
    if (!std::getline(std::cin >> std::ws, input)) throw EndOfInput();
    pending_input_line() = false;
    return input;
}

inline void color(unsigned short attribute) {
    if (!output_is_terminal()) return;
#ifdef _WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attribute);
#else
    static const int colors[] = {0, 4, 2, 6, 1, 5, 3, 7};
    int foreground = 30 + colors[attribute & 7] + ((attribute & 8) ? 60 : 0);
    int background = 40 + colors[(attribute >> 4) & 7] + ((attribute & 128) ? 60 : 0);
    std::cout << "\033[" << foreground << ';' << background << 'm';
#endif
}

inline void reset() {
    if (!output_is_terminal()) return;
#ifdef _WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
#else
    std::cout << "\033[0m" << std::flush;
#endif
}

inline void clear() {
    if (!output_is_terminal()) return;
#ifdef _WIN32
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (!GetConsoleScreenBufferInfo(console, &info)) return;
    COORD origin = {0, 0};
    DWORD written;
    DWORD cells = static_cast<DWORD>(info.dwSize.X) * info.dwSize.Y;
    FillConsoleOutputCharacterA(console, ' ', cells, origin, &written);
    FillConsoleOutputAttribute(console, info.wAttributes, cells, origin, &written);
    SetConsoleCursorPosition(console, origin);
#else
    std::cout << "\033[2J\033[H" << std::flush;
#endif
}

inline void pause() {
    if (!input_is_terminal()) return;
    std::cout << "Press Enter to continue . . ." << std::flush;
    if (pending_input_line()) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        pending_input_line() = false;
    }
    std::string input;
    if (!std::getline(std::cin, input)) throw EndOfInput();
}

} // namespace Console

#endif // CONSOLE_H
