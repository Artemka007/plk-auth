#include "standard_io_handler.hpp"

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

std::string StandardIOHandler::read_line(const std::string &prompt) {
    if (!prompt.empty()) std::cout << prompt << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) return "";
    return line;
}

void StandardIOHandler::print(const std::string &message) {
    std::cout << message << std::flush;
}

void StandardIOHandler::println(const std::string &message) {
    std::cout << message << '\n';
}

void StandardIOHandler::error(const std::string &message) {
    std::cerr << "Error: " << message << '\n';
}

std::string StandardIOHandler::read_password(const std::string &prompt) {
    std::string password;
    std::cout << prompt << std::flush;

#ifdef _WIN32
    char ch;
    while ((ch = _getch()) != '\r') { // Read until Enter
        if (ch == '\b') {             // Handle Backspace
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
    new_termios.c_lflag &= ~ECHO; // Disable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    std::getline(std::cin, password);

    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    std::cout << std::endl;
#endif
    return password;
}

bool StandardIOHandler::is_eof() const { return std::cin.eof(); }

CommandArgs StandardIOHandler::parse_command(const std::string &input) const {
    CommandArgs args;
    std::istringstream iss(input);
    std::string token;
    bool in_quotes = false;
    std::string current;

    for (char ch : input) {
        if (ch == '"') { // Toggle quotes
            in_quotes = !in_quotes;
        } else if (isspace(ch) && !in_quotes) {
            if (!current.empty()) {
                if (current.rfind("--", 0) == 0) {
                    auto pos = current.find('=');
                    if (pos != std::string::npos) {
                        args.options[current.substr(2, pos - 2)] = current.substr(pos + 1);
                    } else {
                        args.flags.push_back(current.substr(2));
                    }
                } else if (current.rfind("-", 0) == 0 && current.size() > 1) { // -f
                    for (size_t i = 1; i < current.size(); ++i)
                        args.flags.push_back(std::string(1, current[i]));
                } else {
                    args.positional.push_back(current);
                }
                current.clear();
            }
        } else {
            current += ch;
        }
    }

    if (!current.empty()) {
        // Handle last token
        if (current.rfind("--", 0) == 0) {
            auto pos = current.find('=');
            if (pos != std::string::npos) {
                args.options[current.substr(2, pos - 2)] = current.substr(pos + 1);
            } else {
                args.flags.push_back(current.substr(2));
            }
        } else if (current.rfind("-", 0) == 0 && current.size() > 1) {
            for (size_t i = 1; i < current.size(); ++i)
                args.flags.push_back(std::string(1, current[i]));
        } else {
            args.positional.push_back(current);
        }
    }

    return args;
}