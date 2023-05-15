#pragma once

#include <cstddef>
#include <new>

#include "AdapterAllocator.h"

class HeapAllocator {
 public:
    static void* allocate(std::size_t size) {
        return ::operator new(size);
    }

    static void deallocate(std::size_t size, void* data) {
        ::operator delete(data);
    }
};

template <typename T>
using StdHeapAllocator = AdapterAllocator<T, HeapAllocator>;
