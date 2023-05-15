#pragma once

#include <stdexcept>
#include <format>
#include <cstddef>
#include <memory>
#include <new>

#include "AdapterAllocator.h"

template <std::size_t s>
struct EqualsCondition {
    static bool match(std::size_t size) {
        return size == s;
    }
};

template <typename Alloc1, typename Alloc2, typename Condition1, typename Condition2>
class ConditionalAllocator {
 public:
    static void* allocate(std::size_t size) {
        if (Condition1::match(size)) {
            return Alloc1::allocate(size);
        } else if (Condition2::match(size)) {
            return Alloc2::allocate(size);
        } else {
            throw std::runtime_error(std::format("both conditions are false in ConditionalAllocator::allocate(size = {})", size));
        }
    }

    static void deallocate(std::size_t size, void* data) {
        if (Condition1::match(size)) {
            return Alloc1::deallocate(size, data);
        } else if (Condition2::match(size)) {
            return Alloc2::deallocate(size, data);
        } else {
            throw std::runtime_error(std::format("both conditions are false in ConditionalAllocator::deallocate(size = {})", size));
        }
    }
};

template <typename T, typename Alloc1, typename Alloc2, typename Condition1, typename Condition2>
using StdConditionalAllocator = AdapterAllocator<T, ConditionalAllocator<Alloc1, Alloc2, Condition1, Condition2>>;
