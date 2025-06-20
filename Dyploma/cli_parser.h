#pragma once
#include <vector>
#include <string>

struct CLIArgs {
    std::vector<std::string> files;
    std::vector<std::string> patterns;
};

CLIArgs parseArgs(int argc, char* argv[]) {
    // Можно потом расширить: пока не используется
    return {};
}
