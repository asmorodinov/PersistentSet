# CMakeList.txt : CMake project for PersistentSet, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("PersistentSet")

# Add source to this project's executable.
add_executable (PersistentSet "main.cpp"  "Allocators/AdapterAllocator.h" "Allocators/ConditionalAllocator.h" "Allocators/FreeListAllocator.h" "Allocators/HeapAllocator.h" "Allocators/PoolAllocator.h" "Allocators/TwoPoolsAllocator.h" "External/memory-allocators/src/PoolAllocator.cpp")

# todo - use target_link_libraries, instead of add_executable + target_include_directories
target_include_directories(PersistentSet PRIVATE "External/memory-allocators/includes")

# only used for immer free list allocator, feel free to remove this dependency
find_package(Immer CONFIG REQUIRED)
target_link_libraries(PersistentSet PRIVATE immer)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET PersistentSet PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
