#pragma once

#include <concepts>
#include <memory>

#include "PatriciaTree.h"

namespace patricia {

template <Unsigned T, Unsigned Bitmap, typename Alloc = std::allocator<detail::IntPatriciaNode<T, Bitmap>>>
class IntSet {
 public:
    IntSet() = default;

    bool contains(T key) const {
        return detail::Lookup<T, Bitmap>(t, key);
    }

    void insert(T key) {
        t = detail::Insert<T, Bitmap, Alloc>(t, key);
    }

    void clear() {
        t = {};
    }

 private:
    detail::IntPatriciaPtr<T, Bitmap> t;
};

}  // namespace patricia
