#pragma once

#include <cstddef>
#include <limits>
#include <memory>

template <class T, class Allocator>
struct AdapterAllocator {
    using value_type = T;

    AdapterAllocator() = default;

    template <class U, class Alloc>
    constexpr AdapterAllocator(const AdapterAllocator<U, Alloc>&) noexcept {
    }

    [[nodiscard]] T* allocate(std::size_t n) {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
            throw std::bad_array_new_length();
        }

        if (auto p = static_cast<T*>(Allocator::allocate(n * sizeof(T)))) {
            return p;
        }

        throw std::bad_alloc();
    }

    void deallocate(T* p, std::size_t n) noexcept {
        Allocator::deallocate(n * sizeof(T), p);
    }
};

template <class T, class U, class AllocatorLhs, class AllocatorRhs>
bool operator==(const AdapterAllocator<T, AllocatorLhs>&, const AdapterAllocator<U, AllocatorRhs>&) {
    return true;
}

template <class T, class U, class AllocatorLhs, class AllocatorRhs>
bool operator!=(const AdapterAllocator<T, AllocatorLhs>&, const AdapterAllocator<U, AllocatorRhs>&) {
    return false;
}
