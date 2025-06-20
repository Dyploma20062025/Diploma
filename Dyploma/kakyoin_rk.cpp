#include "kakyoin_rk.h"
#include "comparison_counter.h"

static const int kBase = 256;
static const int kMod = 101;

std::vector<int> kakyoinRabinKarpSearch(const std::string& text, const std::string& pattern) {
    std::vector<int> matches;
    int n = (int)text.size();
    int m = (int)pattern.size();

    if (m == 0 || n < m) return matches;

    int pHash = 0, tHash = 0, h = 1;
    for (int i = 0; i < m - 1; ++i)
        h = (h * kBase) % kMod;

    for (int i = 0; i < m; ++i) {
        pHash = (kBase * pHash + pattern[i]) % kMod;
        tHash = (kBase * tHash + text[i]) % kMod;
    }

    for (int i = 0; i <= n - m; ++i) {
        if (pHash == tHash) {
            bool match = true;
            for (int j = 0; j < m; ++j) {
                JojoComparisonCounter::add();
                if (text[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) matches.push_back(i);
        }
        if (i < n - m) {
            tHash = (kBase * (tHash - text[i] * h) + text[i + m]) % kMod;
            if (tHash < 0) tHash += kMod;
        }
    }
    return matches;
}
