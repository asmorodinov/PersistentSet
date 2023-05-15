#pragma once

#include <cstddef>
#include <new>

#include <immer/heap/cpp_heap.hpp>
#include <immer/heap/heap_policy.hpp>

#include "AdapterAllocator.h"

template <std::size_t Size>
using FreeListAllocator = immer::unsafe_free_list_heap_policy<immer::cpp_heap>::optimized<Size>::type;

template <typename T, std::size_t Size>
using StdFreeListAllocator = AdapterAllocator<T, FreeListAllocator<Size>>;
