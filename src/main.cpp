#include <fcntl.h>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>

#include "main.hpp"

using namespace pmem::obj;
using namespace jeb_pmem;

#define LAYOUT_NAME "jasobrown_layout"

template<typename T>
PoolWrapper<T>::PoolWrapper(PoolWrapper&& wrapper) noexcept {
    pool_ = wrapper.pool_;
    wrapper.pool_ = nullptr;
}

template<typename T>
PoolWrapper<T> &PoolWrapper<T>::operator=(PoolWrapper<T>&& wrapper) noexcept {
    pool_ = wrapper.pool_;
    wrapper.pool_ = nullptr;
    return *this;
}

template<typename T>
PoolWrapper<T>::~PoolWrapper() noexcept {
    if (pool_ != nullptr) {
        pool_.close();
    }
}

std::shared_ptr<PoolWrapper<PoolRoot>> createPool(const std::string &path, size_t size) {

    auto pop = pool<PoolRoot>::create(path, LAYOUT_NAME, size);

    auto poolRoot = pop.root();
    poolRoot->userRoot = 0;
    poolRoot->intendedHeapSize = 0;
    // persist the root obj now!
    return std::make_shared<PoolWrapper<PoolRoot>>(pop);
}

std::shared_ptr<PoolWrapper<PoolRoot>> loadPool(const std::string &path, size_t size) {
    auto pop = pool<PoolRoot>::open(path, LAYOUT_NAME);
    // read existing root object
    // assert that stored size == size argument
    return std::make_shared<PoolWrapper<PoolRoot>>(pop);
}

int main() {

    // *1* create an array of pools, one per each NUMA node
    
    // do a check to see if file exists, instead of blindly creating
    auto pool = createPool("/mnt/mem/pmem-pooling-example", PMEMOBJ_MIN_POOL);

    // ok, we have an open pool(s)
    // now, set (or read) the user root object 

    //   allocate the 16-byte block, and write the values to it


    // *2* now create a pool of threads, with affinity set per CPU
    // (related to NUMA node)
    
    return 0;
}
