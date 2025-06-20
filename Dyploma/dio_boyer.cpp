#include "dio_boyer.h"
#include <unordered_map>
#include "comparison_counter.h"

static std::unordered_map<char, int> dioBadCharTable(const std::string& pattern) {
    std::unordered_map<char, int> table;
    for (int i = 0; i < (int)pattern.size(); ++i) {
        table[pattern[i]] = i;
    }
    return table;
}

std::vector<int> dioBoyerMooreSearch(const std::string& text, const std::string& pattern) {
    std::vector<int> results;
    if (pattern.empty() || text.size() < pattern.size()) return results;

    auto badChar = dioBadCharTable(pattern);
    int n = (int)text.size();
    int m = (int)pattern.size();

    int s = 0;
    while (s <= n - m) {
        int j = m - 1;
        while (j >= 0) {
            JojoComparisonCounter::add();
            if (pattern[j] != text[s + j]) break;
            j--;
        }
        if (j < 0) {
            results.push_back(s);
            s += (s + m < n) ? m - (badChar.count(text[s + m]) ? badChar[text[s + m]] : -1) : 1;
        }
        else {
            int bcShift = badChar.count(text[s + j]) ? badChar[text[s + j]] : -1;
            s += std::max(1, j - bcShift);
        }
    }
    return results;
}
