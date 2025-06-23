#pragma once
class JojoComparisonCounter {
public:
    static void reset();
    static void add(int count = 1);
    static int get();
private:
    static inline int counter = 0;
};
