#pragma once

#pragma once

#include <cstddef>
#include <memory>
#include <new>

#include "AdapterAllocator.h"
#include "ConditionalAllocator.h"
#include "PoolAllocator.h"

template <std::size_t Count, std::size_t Size1, std::size_t Size2>
using TwoPoolsAllocator =
    ConditionalAllocator<StaticPoolAllocator<Count, Size1>, StaticPoolAllocator<Count, Size2>, EqualsCondition<Size1>, EqualsCondition<Size2>>;

template <typename T, std::size_t Count, std::size_t Size1, std::size_t Size2>
using StdTwoPoolsAllocator = AdapterAllocator<T, TwoPoolsAllocator<Count, Size1, Size2>>;
