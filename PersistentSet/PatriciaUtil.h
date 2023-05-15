#pragma once

#include <concepts>
#include <limits>

namespace patricia {

template <typename T>
concept Unsigned = std::unsigned_integral<T>;

namespace util {

template <Unsigned T>
T highBitsOfKey(T key, T mask) {
    return key & ((~(mask - 1)) ^ mask);
}

template <Unsigned T>
bool branchingBitIsZero(T key, T mask) {
    return (key & mask) == 0;
}

template <Unsigned T>
bool matchBranch(T key, T prefix, T mask) {
    return highBitsOfKey(key, mask) == prefix;
}

// Implementation is based on http://aggregate.org/MAGIC/#Most%20Significant%201%20Bit
// Alternative implementations:
// https://stackoverflow.com/questions/671815/what-is-the-fastest-most-efficient-way-to-find-the-highest-set-bit-msb-in-an-i
// Note: In practice, this function does not appear to significally affect performance of IntPatricia, since it's used at most once per insert
template <Unsigned T>
T highestBitMask(T x) {
    // This for loop will hopefully be unrolled by compiler (gcc with -O3 does unroll it)
    for (int shift = 1; shift < std::numeric_limits<T>::digits; shift *= 2) {
        x |= (x >> shift);
    }
    return x & ~(x >> 1);
}

// return the mask of the longest common prefix
template <Unsigned T>
T branchMask(T p1, T p2) {
    return highestBitMask(p1 ^ p2);
}

// prefix and bitmap helper functions

// note:
// here, by prefix of the key we mean "all but last 5-6 bits of the key"
// suffix is last 5-6 bits of the key
// bitmap is suffix stored as bitmask
// more info can be found in the source code of https://hackage.haskell.org/package/containers-0.6.7/docs/Data-IntSet.html

constexpr size_t log2(int n) {
    return n <= 1 ? 0 : 1 + log2(n / 2);
}

template <Unsigned Bitmap>
constexpr size_t numberOfBitsInBitmap = log2(std::numeric_limits<Bitmap>::digits);

template <Unsigned T, Unsigned Bitmap>
constexpr T suffixBitMask = std::numeric_limits<Bitmap>::digits - 1;

template <Unsigned T, Unsigned Bitmap>
constexpr T prefixBitMask = ~suffixBitMask<T, Bitmap>;

template <Unsigned T, Unsigned Bitmap>
T prefixOf(T key) {
    return key & prefixBitMask<T, Bitmap>;
}

template <Unsigned T, Unsigned Bitmap>
T suffixOf(T key) {
    return key & suffixBitMask<T, Bitmap>;
}

template <Unsigned T, Unsigned Bitmap>
Bitmap bitmapOfSuffix(T suffix) {
    return static_cast<Bitmap>(1) << suffix;
}

template <Unsigned T, Unsigned Bitmap>
Bitmap bitmapOf(T key) {
    return bitmapOfSuffix<T, Bitmap>(suffixOf<T, Bitmap>(key));
}

template <Unsigned T, Unsigned Bitmap>
bool matchLeaf(T key, T prefix, Bitmap bitmap) {
    return (prefixOf<T, Bitmap>(key) == prefix) && ((bitmapOf<T, Bitmap>(key) & bitmap) != 0);
}

}  // namespace util
}  // namespace patricia
