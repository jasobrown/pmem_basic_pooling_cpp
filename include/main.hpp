#pragma once

#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
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
    p<uint64_t> userRootOffset;
    p<uint64_t> heapSize;
    //    p<unsigned char[16]> data;
    //    p<unsigned char> data[16];

    // TODO(jeb): decide what to do with ctor/copy ctor/move, etc...
    
    size_t GetUserRootOffset();
    void SetUserRootOffset(size_t size);

    /// Retrieve the size of the heap as declared when the heap
    /// was initially created.
    size_t GetHeapSize();
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
