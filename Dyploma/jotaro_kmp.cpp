#include "jotaro_kmp.h"
#include "comparison_counter.h"

static std::vector<int> jotaroPrefixFunc(const std::string& s) {
    std::vector<int> p(s.size(), 0);
    int k = 0;
    for (size_t i = 1; i < s.size(); ++i) {
        while (k > 0 && s[i] != s[k]) {
            JojoComparisonCounter::add();
            k = p[k - 1];
        }
        JojoComparisonCounter::add();
        if (s[i] == s[k]) ++k;
        p[i] = k;
    }
    return p;
}

std::vector<int> jotaroKMPsearch(const std::string& text, const std::string& pattern) {
    std::vector<int> found_positions;
    if (pattern.empty() || text.size() < pattern.size()) return found_positions;

    std::vector<int> prefix = jotaroPrefixFunc(pattern);
    int j = 0;

    for (size_t i = 0; i < text.size(); ++i) {
        while (j > 0 && text[i] != pattern[j]) {
            JojoComparisonCounter::add();
            j = prefix[j - 1];
        }
        JojoComparisonCounter::add();
        if (text[i] == pattern[j]) ++j;
        if (j == (int)pattern.size()) {
            found_positions.push_back((int)i - j + 1);
            j = prefix[j - 1];
        }
    }
    return found_positions;
}
