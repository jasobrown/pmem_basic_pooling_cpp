#pragma once

#include <libpmemobj++/pool.hpp>

using namespace pmem::obj;

namespace jeb_pmem {

/// Modeled after the pool root structure as implemented
/// the java llpl library. (Note: this format isn't documented
/// in the llpl, but here we are.) The format contains two
/// unsigned 64-bit values: one that points to the user's
/// definded root object, and one that declares the intended
/// size of the heap. 
class PoolRoot {
  public:
    // Pointer to 'user object root' addr.
    size_t userRootOffset;

    size_t intendedHeapSize;
};

class PoolUserRoot {
  public:
    uint64_t val1;
    int64_t val2;
};

/// A wrapper for the pmem pool such that we can automatically invoke
/// pool.close() when the ref count hits zero (assuming std::shared_ptr).
template<typename T>
class PoolWrapper {
  public:
    pool<T> pool_;

    PoolWrapper(pool<T> pool) : pool_(pool) {};

    // copy ctor
    PoolWrapper(PoolWrapper<T> &wrapper) = delete;

    // assignment operator
    PoolWrapper &operator=(PoolWrapper<T> &wrapper) = delete;
    
    // move ctor
    PoolWrapper(PoolWrapper<T>&& wrapper) noexcept;

    // move assignment
    PoolWrapper<T> &operator=(PoolWrapper<T>&& wrapper) noexcept;
    
    ~PoolWrapper() noexcept;
};

} //namespace jeb_pmem
