#pragma once
#include <string>

struct ValidationResult {
    bool valid = true;
    std::string error_message;
};
