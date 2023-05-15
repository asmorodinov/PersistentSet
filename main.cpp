#include <cassert>
#include <cstdint>
#include <iostream>

#include "PersistentSet/PatriciaSet.h"

int main()
{
    // simple persistent set example

    patricia::IntSet<std::uint32_t, std::uint64_t> set;
    
    // set = {0, 1, ..., 41}
    for (std::uint32_t i = 0; i < 42; ++i) {
        set.insert(i);
        assert(set.contains(i));
    }

    // set2 = {0, 1, ..., 41}
    auto set2 = set;

    // set = {0, ..., 40, 41, 42}
    // set2 = {0, ..., 40, 41, 43}
    set.insert(42);
    set2.insert(43);

    for (std::uint32_t i = 0; i < 44; ++i) {
        if (i < 42) {
            assert(set.contains(i));
            assert(set2.contains(i));
        } else if (i == 42) {
            assert(set.contains(i));
            assert(!set2.contains(i));
        } else if (i == 43) {
            assert(!set.contains(i));
            assert(set2.contains(i));
        }
    }

    // set = {}
    // set2 = {0, ..., 40, 41, 43}
    set.clear();
    for (std::uint32_t i = 0; i < 42; ++i) {
        assert(!set.contains(i));
        assert(set2.contains(i));
    }

    std::cout << "All tests passed\n";

    return 0;
}
