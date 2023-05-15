#pragma once

#include <cstddef>
#include <memory>
#include <new>

#include "AdapterAllocator.h"

#include "../External/memory-allocators/includes/PoolAllocator.h"

template <std::size_t Count, std::size_t ChunkSize>
class StaticPoolAllocator {
 public:
    static void* allocate(std::size_t size) {
        return GetAllocator()->Allocate(size);
    }

    static void deallocate(std::size_t size, void* data) {
        GetAllocator()->Free(data);
    }

    static PoolAllocator* GetAllocator() {
        static std::unique_ptr<PoolAllocator> allocator;

        if (!allocator) {
            allocator = std::make_unique<PoolAllocator>(Count * ChunkSize, ChunkSize);
            allocator->Init();
        }

        return allocator.get();
    }
};

template <typename T, std::size_t Count, std::size_t ChunkSize>
using StdPoolAllocator = AdapterAllocator<T, StaticPoolAllocator<Count, ChunkSize>>;
