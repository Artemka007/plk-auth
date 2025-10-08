#pragma once

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/rand.h>
#include <sstream>
#include <string>
#include <iostream>

namespace utils {
class PasswordUtils {
public:
    // Используем PBKDF2 для более безопасного хеширования паролей
    static std::string hash_password_pbkdf2(const std::string &password, const std::string &salt = "") {
        std::string actual_salt = salt.empty() ? generate_salt(16) : salt;

        std::vector<unsigned char> hash(32); // 256 бит = 32 байта
        const int iterations = 100000; // Количество итераций

        if (PKCS5_PBKDF2_HMAC(
            password.c_str(), password.length(),
            reinterpret_cast<const unsigned char*>(actual_salt.c_str()), actual_salt.length(),
            iterations,
            EVP_sha256(),
            hash.size(), hash.data()) != 1) {
            throw std::runtime_error("Failed to generate PBKDF2 hash");
        }

        std::stringstream ss;
        ss << actual_salt << ":";
        for (unsigned char byte : hash) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        return ss.str();
    }

    static bool verify_password_pbkdf2(const std::string &password, const std::string &stored_hash) {
        // Формат: salt:hash
        size_t separator_pos = stored_hash.find(':');
        if (separator_pos == std::string::npos) {
            return false;
        }

        std::string salt = stored_hash.substr(0, separator_pos);
        std::string computed_hash = hash_password_pbkdf2(password, salt);
        return computed_hash == stored_hash;
    }

    static std::string generate_salt(size_t length = 16) {
        std::vector<unsigned char> salt_bytes(length);
        if (RAND_bytes(salt_bytes.data(), length) != 1) {
            throw std::runtime_error("Failed to generate salt");
        }

        std::stringstream ss;
        for (size_t i = 0; i < length; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(salt_bytes[i]);
        }
        return ss.str();
    }

    // Простая SHA256 хеш-функция (для обратной совместимости)
    static std::string hash_password_sha256(const std::string &password) {
        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
        if (!ctx) {
            throw std::runtime_error("Failed to create EVP_MD_CTX");
        }

        if (EVP_DigestInit_ex(ctx.get(), EVP_sha256(), nullptr) != 1) {
            throw std::runtime_error("Failed to initialize digest");
        }

        if (EVP_DigestUpdate(ctx.get(), password.c_str(), password.size()) != 1) {
            throw std::runtime_error("Failed to update digest");
        }

        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int hash_len = 0;
        if (EVP_DigestFinal_ex(ctx.get(), hash, &hash_len) != 1) {
            throw std::runtime_error("Failed to finalize digest");
        }

        std::stringstream ss;
        for (unsigned int i = 0; i < hash_len; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }
        return ss.str();
    }

    static bool verify_password_sha256(const std::string &password, const std::string &hash) {
        return hash_password_sha256(password) == hash;
    }

    static bool is_password_strong(const std::string &password) {
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

    static std::string generate_random_password(size_t length) {
        const std::string chars =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
        std::string password;
        password.reserve(length);

        std::vector<unsigned char> random_bytes(length);
        if (RAND_bytes(random_bytes.data(), length) != 1) {
            throw std::runtime_error("Failed to generate random bytes");
        }

        for (size_t i = 0; i < length; ++i) {
            password += chars[random_bytes[i] % chars.length()];
        }

        return password;
    }
};
} // namespace utils
