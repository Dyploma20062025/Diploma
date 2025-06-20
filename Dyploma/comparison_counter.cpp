#include "comparison_counter.h"

void JojoComparisonCounter::reset() {
    counter = 0;
}

void JojoComparisonCounter::add(int count) {
    counter += count;
}

int JojoComparisonCounter::get() {
    return counter;
}
