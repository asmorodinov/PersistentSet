# PersistentSet

This repository contains C++ implementation of a persistent integer set based on integer Patricia tree data structure.

## Features
- Persistency.
- Big-endian order of traversal.
- Support for bitmaps in the leaves of the tree.
- Can store keys of any unsigned integer type.
- Bitmaps can also be any unsigned integer types.
- Allocator support (I would recommend to use [TwoPoolsAllocator](Allocators/TwoPoolsAllocator.h) - one pool for leaf nodes, and one for branch nodes).
- **Not** thread safe (but it's probably not very hard to implement thread-safe version).
- No `erase` and `union` support yet.

## Examples

Examples of how to use this data structure are provided in [main.cpp](main.cpp) file.

## Interface

`IntSet` class defined in [PatriciaSet.h](PersistentSet/PatriciaSet.h) implements following methods:

- `IntSet()` - constructs empty set
- `void insert(T key)` - inserts `key` into set. Note that `T` must be an unsigned integer type (satisfy `std::unsigned_integral<T>` concept).
- `bool contains(T key) const` - checks whether `key` is present in the set, or not (i.e. `key` was previously inserted).
- `void clear()` - clears the set.
- Copy-constructor, assignment operator, etc are generated by compiler.

As you can see, `void erase(T key)` is not implemented yet, but pull requests are welcome (in my use case, `erase` method was not required, so it was not implemented).

Other methods, such as `IntSet union(IntSet other) const` (that returns union of two sets), should also be relatively easy to implement.

## Performance
`insert`, `contains` have worst-case complexity of `O(min(n, W))`, where `n` is number of elements stored in a set, `W` is number of bits in `T`. In practice, it's reasonable to assume that they have complexity `O(1)`.

`clear` has worst-case complexity of `O(n)` when it needs to delete all of the elements in the set.

Copying the data-structure has worst-case complexity of `O(1)`.

Performance was not tested via microbenchmarks, however, in my use case it performed better than existing implementations of Patricia trees ([NASA-SW-VnV/ikos](https://github.com/NASA-SW-VnV/ikos/tree/master/core/include/ikos/core/adt/patricia_tree) and [facebook/sparta](https://github.com/facebook/SPARTA/blob/main/include/PatriciaTreeCore.h)) - see [performance.md](https://github.com/asmorodinov/3DRoguelike/blob/master/performance.md#results) (3d vs 4th and 5th columns).

## How to build

Originally, Microsoft Visual Studio 2022 was used to build this project (as a CMake project).

However, code should be crossplatform, and it should be possible to build this project using other methods (with CMake).

Alternatively, you can simply copy header files from `PersistentSet` and `Allocators` folders, as well as `External` folder (make sure to use `git clone` with `--recurse-submodules` option) and import them into your project. 

Using custom allocators from `Allocators` folder is optional, but is recommended for performance reasons.

## Dependencies
The only depencency is [Immer](https://github.com/arximboldi/immer), and it's only purpose is to provide implementation for [FreeListAllocator](Allocators/FreeListAllocator.h). Feel free to remove this dependency, if you choose not to use FreeListAllocator (and for example use [TwoPoolsAllocator](Allocators/TwoPoolsAllocator.h) instead).

Originally, immer was installed using vcpkg.

## Implementation
Code is based on Haskell's [Data.IntSet](https://hackage.haskell.org/package/containers-0.6.7/docs/Data-IntSet.html) implementation.

## Alternatives
- [NASA-SW-VnV/ikos](https://github.com/NASA-SW-VnV/ikos/tree/master/core/include/ikos/core/adt/patricia_tree) - good implementation, but uses little-endian key traversal, and no support for bitmaps in leaves and allocators (so it's a little bit slower).
- [facebook/sparta](https://github.com/facebook/SPARTA/blob/main/include/PatriciaTreeCore.h) - similar to ikos.
- [sikol/patricia](https://github.com/sikol/patricia) - contains bugs as far as I know
- [libstdc++/pat_trie_.hpp](https://gcc.gnu.org/onlinedocs/gcc-4.9.2/libstdc++/api/a01084_source.html) - not sure how to build this and include in your own project.
- [arximboldi/immer](https://github.com/arximboldi/immer) - `immer::set`. Actually, it is implemented with CHAMP (Compressed Hash-Array Mapped Prefix-tree) and not Patricia tree, so it's more similar to [Data.HashSet](https://hackage.haskell.org/package/unordered-containers-0.2.19.1/docs/Data-HashSet.html) than [Data.IntSet](https://hackage.haskell.org/package/containers-0.6.7/docs/Data-IntSet.html). However, it is very well implemented, and actually performs better than this implementation. It also supports `erase` operation, and non-integer keys. However, `union` operation would probably be more expensive for CHAMP compared to Patricia trees.
- [arximboldi/immer](https://github.com/arximboldi/immer) - `immer::vector`. It is implemented using Relaxed Radix Balanced Trees (see authors [video](https://youtu.be/y_m0ce1rzRI) and [paper](https://dl.acm.org/doi/pdf/10.1145/3110260) on the topic). If the keys are in some continuous range (e.g $0, \ldots, n - 1$), then you can store set of them in a `vector<bool>` of size $n$ ($x \in s \iff s[x] == true$), or `vector<uint64_t>` of size $n / 64$. This is the most efficient approach according to our benchmarks (see [performance.md](https://github.com/asmorodinov/3DRoguelike/blob/master/performance.md#results), 1st column).

## Resources on Patricia trees

### Papers

- I recommend to read the [paper](http://web.archive.org/web/20080916143459/https://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.37.5452):
  - Chris Okasaki and Andy Gill, "Fast Mergeable Integer Maps", Workshop on ML, September 1998, pages 77-86

  It's relatively short, explains Patricia trees very well, and includes implementations of methods in Standard ML language.

- Original [paper](https://dl.acm.org/doi/10.1145/321479.321481) on Patricia trees:
  - D.R. Morrison, "PATRICIA -- Practical Algorithm To Retrieve Information Coded In Alphanumeric", Journal of the ACM, 15(4), October 1968, pages 514-534.

### Books

- https://github.com/liuxinyu95/AlgoXY/files/11413742/algoxy-en.pdf - great book on algorithms and data structures, also contains description (and sample implementation) of Patricia trees.

### Videos
- [Lambda World 2018 - The Radix Trees How IntMap Works - Tikhon Jelvis](https://youtu.be/0udjkEiCjog)
- [C++Now 2017: Phil Nash “The Holy Grail!? A Persistent Hash-Array-Mapped Trie for C++"](https://youtu.be/WT9kmIE3Uis) - about HAMT (not Patricia tree)

### Other
- [Functional Programming in C++](https://github.com/graninas/cpp_functional_programming)
- [libstdc++ Chapter 21. Policy-Based Data Structures
Prev 	Part III.  Extensions](https://gcc.gnu.org/onlinedocs/libstdc++/manual/policy_data_structures.html)
- [libstdc++ Trie design](https://gcc.gnu.org/onlinedocs/libstdc++/manual/policy_data_structures_design.html#pbds.design.container.trie)

## Patricia tree in other languages
- Haskell
  - https://hackage.haskell.org/package/containers-0.6.7/docs/Data-IntSet.html
  - https://github.com/haskell/containers/blob/master/containers/src/Data/IntSet/Internal.hs
  - https://github.com/haskell-perf/sets - benchmarks
- OCaml 
  - https://github.com/backtracking/ptset
  - http://cristal.inria.fr/~frisch/icfp06_contest/advtr/applyOmatic/ptset.ml
  - https://www.lri.fr/~filliatr/ftp/ocaml/misc/ptset.ml
- Clojure 
  - https://github.com/clojure/data.int-map
  - https://github.com/ztellman/immutable-int-map
- Python https://github.com/DRMacIver/intset
- Go https://pkg.go.dev/golang.org/x/tools/go/callgraph/vta/internal/trie
- Scala 
  - https://www.scala-lang.org/api/2.12.9/scala/collection/immutable/IntMap.html
  - https://github.com/scala/scala/blob/v2.12.9/src/library/scala/collection/immutable/IntMap.scala#L166
- Scheme https://srfi.schemers.org/srfi-224/srfi-224.html
- Java 
  - https://lacuna.io/docs/bifurcan/io/lacuna/bifurcan/IntMap.html 
  - https://github.com/lacuna/bifurcan
- Lisp https://github.com/fare/lisp-interface-library/blob/master/pure/fmim-implementation.lisp
