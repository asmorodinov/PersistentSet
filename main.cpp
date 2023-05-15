#include <cassert>
#include <cstdint>
#include <iostream>

#include "PersistentSet/PatriciaSet.h"
#include "Allocators/TwoPoolsAllocator.h"
#include "Allocators/HeapAllocator.h"
#include "Allocators/FreeListAllocator.h"

template <patricia::Unsigned T, patricia::BitmapC Bitmap, typename Alloc = std::allocator<patricia::detail::IntPatriciaNode<T, Bitmap>>>
void SimpleTest() {
    // simple persistent set example

    patricia::IntSet<T, Bitmap, Alloc> set;

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
void SimpleAllocatorTest() {
    patricia::IntSet<std::uint64_t, std::uint64_t, Alloc> set;

    set.insert(1);
    assert(set.contains(1));
    set.clear();
    assert(!set.contains(1));
}

int main()
{
    SimpleTest<std::uint32_t, std::uint32_t>();
    SimpleTest<std::uint32_t, std::uint64_t>();
    SimpleTest<std::uint32_t, patricia::NoBitmap>();
    SimpleTest<std::uint32_t, patricia::NoBitmap, StdTwoPoolsAllocator<void, 1 << 10, 72, 40>>();

    SimpleAllocatorTest<StdTwoPoolsAllocator<void, 1 << 10, 72, 48>>();
    SimpleAllocatorTest<StdFreeListAllocator<void, 72>>();
    SimpleAllocatorTest<StdHeapAllocator<void>>();
    SimpleAllocatorTest<std::allocator<void>>();

    std::cout << "All tests passed\n";

    return 0;
}
