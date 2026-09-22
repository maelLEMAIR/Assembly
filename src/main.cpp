#include <iostream>

extern "C" {
    int64_t asm_add(int64_t a, int64_t b);
    int64_t asm_maxInAnArray(int64_t* a, int64_t b);
    int64_t* asm_sortAnArray(int64_t* a, int64_t b);
} 

int main() {
    int64_t tab[] = {4, 2, 50, 8, 1, 12};
    asm_sortAnArray(tab, 6);
    for (long long i : tab)
    {
        std::cout << i << " | ";
    }
    return 0;
}
