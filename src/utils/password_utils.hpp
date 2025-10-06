#pragma once

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <openssl/sha.h>
#include <sstream>
#include <string>

namespace utils {

std::string PasswordUtils::hash_password(const std::string &password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, password.c_str(), password.size());
    SHA256_Final(hash, &ctx);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool PasswordUtils::verify_password(const std::string &password, const std::string &hash) {
    return hash_password(password) == hash;
}

bool PasswordUtils::is_password_strong(const std::string &password) {
    if (password.length() < 8) {
        return false;
    }

    bool has_upper = false, has_lower = false, has_digit = false, has_special = false;
    for (char c : password) {
        if (std::isupper(c))
            has_upper = true;
        else if (std::islower(c))
            has_lower = true;
        else if (std::isdigit(c))
            has_digit = true;
        else if (std::ispunct(c))
            has_special = true;
    }

    return has_upper && has_lower && has_digit && has_special;
}

std::string PasswordUtils::generate_random_password(size_t length) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUV"
                              "WXYZ0123456789!@#$%^&*";
    std::string password;
    password.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        password += chars[rand() % chars.length()];
    }

    return password;
}

} // namespace utils
