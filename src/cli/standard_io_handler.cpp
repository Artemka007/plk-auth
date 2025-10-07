#include "cli/standard_io_handler.hpp"

#ifdef _WIN32
#include <conio.h>
#include <iostream>
#else
#include <termios.h>
#include <unistd.h>
#endif

std::string StandardIOHandler::read_password(const std::string &prompt) {
    std::string password;
    std::cout << prompt << std::flush;

#ifdef _WIN32
    char ch;
    while ((ch = _getch()) != '\r') { // Enter
        if (ch == '\b') {             // Backspace
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b" << std::flush;
            }
        } else {
            password.push_back(ch);
            std::cout << '*' << std::flush;
        }
    }
#else
    termios old_termios;
    tcgetattr(STDIN_FILENO, &old_termios);
    termios new_termios = old_termios;
    new_termios.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    std::getline(std::cin, password);

    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    std::cout << std::endl;
#endif

    return password;
}