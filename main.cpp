#include <cassert>
#include <cstdint>
#include <iostream>

#include "PersistentSet/PatriciaSet.h"
#include "Allocators/TwoPoolsAllocator.h"
#include "Allocators/HeapAllocator.h"
#include "Allocators/FreeListAllocator.h"

void SimpleTest() {
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
        }
        else if (i == 42) {
            assert(set.contains(i));
            assert(!set2.contains(i));
        }
        else if (i == 43) {
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
}

template <typename Alloc>
void AllocatorTest() {
    patricia::IntSet<std::uint64_t, std::uint64_t, Alloc> set;

    set.insert(1);
    assert(set.contains(1));
    set.clear();
    assert(!set.contains(1));
}

int main()
{
    SimpleTest();
    AllocatorTest<StdTwoPoolsAllocator<void, 1 << 10, 72, 48>>();
    AllocatorTest<StdFreeListAllocator<void, 72>>();
    AllocatorTest<StdHeapAllocator<void>>();
    AllocatorTest<std::allocator<void>>();

    std::cout << "All tests passed\n";

    return 0;
}
