
#include "bitap_star_platinum.h"
#include "comparison_counter.h"
#include <unordered_map>

std::vector<int> starPlatinumBitapSearch(const std::string& text, const std::string& pattern) {
    std::vector<int> result;
    int m = pattern.length();
    if (m == 0 || m > 63) return result;

    std::unordered_map<char, uint64_t> patternMask;
    for (char c = 0; c < 127; ++c) patternMask[c] = ~0ULL;
    for (int i = 0; i < m; ++i)
        patternMask[pattern[i]] &= ~(1ULL << i);

    uint64_t R = ~1ULL;
    for (int i = 0; i < (int)text.size(); ++i) {
        JojoComparisonCounter::add();
        R |= patternMask[text[i]];
        R <<= 1;
        if ((R & (1ULL << m)) == 0)
            result.push_back(i - m + 1);
    }

    return result;
}
