cmake_minimum_required(VERSION 3.12)
project(pmem-pool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER "/usr/bin/clang-7")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++-7")
set(CMAKE_AR "/usr/bin/llvm-ar-7")
set(CMAKE_LINKER "/usr/bin/lld-7")
set(CMAKE_NM "/usr/bin/llvm-nm-7")
set(CMAKE_OBJDUMP "/usr/bin/llvm-objdump-7")
set(CMAKE_RANLIB "/usr/bin/llvm-ranlib")

add_executable(pmem-pool
  src/main.cpp
)

target_include_directories(pmem-pool
  PUBLIC
  include
  "/opt/dev/cockroach/19.1/native/x86_64-pc-linux-gnu/libpmemobj-cpp/include"
  "/opt/dev/cockroach/19.1/native/x86_64-pc-linux-gnu/libpmemobj-cpp/include"
)

# linking the .so links to a shared library, which is already compiled &
# dynamically locates the deps at runtime
target_link_libraries(pmem-pool
  pthread
  /opt/dev/cockroach/19.1/native/x86_64-pc-linux-gnu/pmdk/src/nondebug/libpmem.so
  /opt/dev/cockroach/19.1/native/x86_64-pc-linux-gnu/pmdk/src/nondebug/libpmemobj.so
  ndctl
  daxctl
  dl
)
