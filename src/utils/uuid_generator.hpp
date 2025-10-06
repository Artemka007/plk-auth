#pragma once
#include <string>
#include <random>
#include <sstream>
#include <iomanip>

namespace utils {

class UUIDGenerator {
private:
    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_int_distribution<> dis_;
    
public:
    UUIDGenerator() : gen_(rd_()), dis_(0, 15) {}
    
    std::string generate() {
        std::stringstream ss;
        ss << std::hex;
        
        for (int i = 0; i < 8; i++) ss << dis_(gen_);
        ss << "-";
        for (int i = 0; i < 4; i++) ss << dis_(gen_);
        ss << "-4"; // version 4
        for (int i = 0; i < 3; i++) ss << dis_(gen_);
        ss << "-";
        ss << (8 + dis_(gen_) % 4); // variant
        for (int i = 0; i < 3; i++) ss << dis_(gen_);
        ss << "-";
        for (int i = 0; i < 12; i++) ss << dis_(gen_);
        
        return ss.str();
    }
    
    static std::string generate_uuid() {
        static UUIDGenerator generator;
        return generator.generate();
    }
};

}