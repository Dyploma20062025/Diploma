#pragma once
#include <string>
#include <vector>
#include <random>

std::string generateWorstCaseString(int length, char ch = 'A') {
    return std::string(length, ch);
}

std::string generateAverageCaseString(int length) {
    static const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<> dist(0, (int)charset.size() - 1);

    std::string result;
    result.reserve(length);
    for (int i = 0; i < length; ++i)
        result += charset[dist(rng)];
    return result;
}
