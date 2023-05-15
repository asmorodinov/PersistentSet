#pragma once

#include <array>
#include <concepts>
#include <limits>
#include <memory>

#include "PatriciaUtil.h"

// This implementation is based on Haskell's Data.IntSet - https://hackage.haskell.org/package/containers-0.6.7/docs/Data-IntSet.html
// Features:
// - Persistency (with mutable interface).
// - Big endian structure.
// - Bitmaps in the leaves of the tree.
// - Can store any unsigned integer type as elements.
// - Bitmaps can also be any unsigned integer types.
// - Allocator support. 
// - I would suggest using allocator with two memory pools: one for leaves nodes, and one for branch nodes.

namespace patricia {
namespace detail {

template <Unsigned T, Unsigned Bitmap>
class IntPatriciaNode;

template <Unsigned T, Unsigned Bitmap>
class IntPatriciaLeaf;

template <Unsigned T, Unsigned Bitmap>
class IntPatriciaBranch;

template <Unsigned T, Unsigned Bitmap>
using IntPatriciaPtr = std::shared_ptr<const IntPatriciaNode<T, Bitmap>>;

template <Unsigned T, Unsigned Bitmap>
using IntPatriciaRawPtr = const IntPatriciaNode<T, Bitmap>*;

template <Unsigned T, Unsigned Bitmap, typename Alloc, typename... Args>
IntPatriciaPtr<T, Bitmap> MakePatriciaLeafPtr(Args&&... args) {
    return std::allocate_shared<const IntPatriciaLeaf<T, Bitmap>>(Alloc(), args...);
}

template <Unsigned T, Unsigned Bitmap, typename Alloc, typename... Args>
IntPatriciaPtr<T, Bitmap> MakePatriciaBranchPtr(Args&&... args) {
    return std::allocate_shared<const IntPatriciaBranch<T, Bitmap>>(Alloc(), args...);
}

template <Unsigned T, Unsigned Bitmap>
class IntPatriciaNode {
 public:
    using Leaf = IntPatriciaLeaf<T, Bitmap>;
    using Branch = IntPatriciaBranch<T, Bitmap>;

    IntPatriciaNode(bool leaf) : isLeaf(leaf) {
    }
    virtual ~IntPatriciaNode() = default;

    bool IsLeaf() const {
        return isLeaf;
    }
    bool IsBranch() const {
        return !isLeaf;
    }

    T GetPrefix() const {
        if (isLeaf) {
            return AsLeaf()->GetPrefix();
        } else {
            return AsBranch()->GetPrefix();
        }
    }

    const Leaf* AsLeaf() const {
        return IsLeaf() ? static_cast<const Leaf*>(this) : nullptr;
    }
    const Branch* AsBranch() const {
        return IsBranch() ? static_cast<const Branch*>(this) : nullptr;
    }

 private:
    // A Patricia tree is an immutable structure.
    IntPatriciaNode(const IntPatriciaNode&) = delete;
    IntPatriciaNode(IntPatriciaNode&&) = delete;
    IntPatriciaNode& operator=(const IntPatriciaNode&) = delete;
    IntPatriciaNode& operator=(IntPatriciaNode&&) = delete;

 private:
    bool isLeaf = false;
};

template <Unsigned T, Unsigned Bitmap>
class IntPatriciaLeaf : public IntPatriciaNode<T, Bitmap> {
 public:
    IntPatriciaLeaf(T key = 0, Bitmap bitmap = 0)
        : IntPatriciaNode<T, Bitmap>(true), prefix(util::prefixOf<T, Bitmap>(key)), bitmap(util::bitmapOf<T, Bitmap>(key) | bitmap) {
    }

    T GetPrefix() const {
        return prefix;
    }
    Bitmap GetBitmap() const {
        return bitmap;
    }

    bool Match(T key) const {
        return util::matchLeaf(key, prefix, bitmap);
    }

 private:
    T prefix;
    Bitmap bitmap;
};

template <Unsigned T, Unsigned Bitmap>
class IntPatriciaBranch : public IntPatriciaNode<T, Bitmap> {
 public:
    IntPatriciaBranch(T p = 0, T m = 0, IntPatriciaPtr<T, Bitmap> l = {}, IntPatriciaPtr<T, Bitmap> r = {})
        : IntPatriciaNode<T, Bitmap>(false), prefix(p), mask(m), left(l), right(r) {
    }

    bool Match(T key) const {
        return util::matchBranch(key, prefix, mask);
    }

    template <typename Alloc>
    IntPatriciaPtr<T, Bitmap> ReplaceChild(IntPatriciaRawPtr<T, Bitmap> x, IntPatriciaPtr<T, Bitmap> y) const {
        if (left.get() == x) {
            return MakePatriciaBranchPtr<T, Bitmap, Alloc>(prefix, mask, y, right);
        } else {
            return MakePatriciaBranchPtr<T, Bitmap, Alloc>(prefix, mask, left, y);
        }
    }

    T GetPrefix() const {
        return prefix;
    }
    T GetMask() const {
        return mask;
    }
    IntPatriciaPtr<T, Bitmap> GetLeft() const {
        return left;
    }
    IntPatriciaPtr<T, Bitmap> GetRight() const {
        return right;
    }

 private:
    T prefix;
    T mask;
    IntPatriciaPtr<T, Bitmap> left;
    IntPatriciaPtr<T, Bitmap> right;
};

template <Unsigned T, Unsigned Bitmap, typename Alloc>
IntPatriciaPtr<T, Bitmap> Branch(T p1, IntPatriciaPtr<T, Bitmap> t1, T p2, IntPatriciaPtr<T, Bitmap> t2) {
    const auto mask = util::branchMask(p1, p2);
    const auto prefix = util::highBitsOfKey(p1, mask);

    if (util::branchingBitIsZero(p1, mask)) {
        return MakePatriciaBranchPtr<T, Bitmap, Alloc>(prefix, mask, t1, t2);
    } else {
        return MakePatriciaBranchPtr<T, Bitmap, Alloc>(prefix, mask, t2, t1);
    }
}

template <Unsigned T, Unsigned Bitmap, typename Alloc>
IntPatriciaPtr<T, Bitmap> Insert(IntPatriciaPtr<T, Bitmap> t, T key) {
    if (!t) {
        // tree is empty
        return MakePatriciaLeafPtr<T, Bitmap, Alloc>(key);
    }

    // maximum depth of Patricia tree
    static constexpr size_t maxDepth = std::numeric_limits<T>::digits - util::numberOfBitsInBitmap<Bitmap>;

    // array that stores path in the tree
    static std::array<IntPatriciaRawPtr<T, Bitmap>, maxDepth> path;
    size_t pathLength = 0;

    // traverse the tree, until key no longer matches prefix, or current node is leaf

    IntPatriciaPtr<T, Bitmap> node = t;
    IntPatriciaPtr<T, Bitmap> parent;

    while (node->IsBranch()) {
        const auto branch = node->AsBranch();
        if (!branch->Match(key)) {
            break;
        }

        parent = node;

        if (util::branchingBitIsZero(key, branch->GetMask())) {
            node = branch->GetLeft();
        } else {
            node = branch->GetRight();
        }

        // store path in the array, since we likely need to copy it later
        path[pathLength++] = node.get();
    }

    IntPatriciaPtr<T, Bitmap> current;

    if (const auto leaf = node->AsLeaf(); leaf && util::matchPrefix<T, Bitmap>(key, leaf->GetPrefix())) {
        // key prefix matches leaf's prefix

        if (util::matchBitmap<T, Bitmap>(key, leaf->GetBitmap())) {
            // full match: key already present
            return t;
        } else {
            // partial match: need to add key's suffix to leaf's bitmap
            current = MakePatriciaLeafPtr<T, Bitmap, Alloc>(key, leaf->GetBitmap());
        }
    } else {
        // key's prefix does not match node's prefix
        // create new branch
        current = Branch<T, Bitmap, Alloc>(node->GetPrefix(), node, util::prefixOf<T, Bitmap>(key), MakePatriciaLeafPtr<T, Bitmap, Alloc>(key));
    }

    if (!parent) {
        return current;
    }

    // path copying
    for (size_t i = pathLength - 1; i-- > 0;) {
        current = path[i]->AsBranch()->ReplaceChild<Alloc>(path[i + 1], current);
    }
    return t->AsBranch()->ReplaceChild<Alloc>(path[0], current);
}

template <Unsigned T, Unsigned Bitmap>
bool Lookup(IntPatriciaPtr<T, Bitmap> t, T key) {
    if (!t) {
        // tree is empty
        return false;
    }

    // search for key
    while (t->IsBranch()) {
        const auto branch = t->AsBranch();

        if (!branch->Match(key)) {
            return false;
        }

        if (util::branchingBitIsZero(key, branch->GetMask())) {
            t = branch->GetLeft();
        } else {
            t = branch->GetRight();
        }
    }

    if (auto leaf = t->AsLeaf(); leaf->Match(key)) {
        return true;
    } else {
        return false;
    }
}

}  // namespace detail
}  // namespace patricia
