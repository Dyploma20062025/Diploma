#include "speedwagon_bitap.h"
#include "comparison_counter.h"

#include <vector>
#include <string>
#include <cstdint>

std::vector<int> speedwagonBitapSearch(const std::string& text, const std::string& pattern) {
    std::vector<int> result;

    int m = (int)pattern.size();
    int n = (int)text.size();

    if (m == 0 || m > 64 || n < m)
        return result;

    uint64_t pattern_mask[256];
    for (int i = 0; i < 256; ++i)
        pattern_mask[i] = ~0ULL;

    for (int i = 0; i < m; ++i)
        pattern_mask[(unsigned char)pattern[i]] &= ~(1ULL << i);

    uint64_t R = ~0ULL;
    uint64_t match_bit = 1ULL << (m - 1);

    for (int i = 0; i < n; ++i) {
        JojoComparisonCounter::add(); 
        R = (R << 1) | pattern_mask[(unsigned char)text[i]];
        if ((~R & match_bit) != 0) {
            result.push_back(i - m + 1);
        }
    }

    return result;
}
