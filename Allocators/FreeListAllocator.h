#pragma once

#include <cstddef>
#include <new>

#include <immer/heap/cpp_heap.hpp>
#include <immer/heap/heap_policy.hpp>

#include "AdapterAllocator.h"

template <typename T>
using FreeListAllocator = immer::unsafe_free_list_heap_policy<immer::cpp_heap, 1u << 20>::optimized<sizeof(T) + 2 * sizeof(void*)>::type;

template <typename T>
using StdFreeListAllocator = AdapterAllocator<T, FreeListAllocator<T>>;
