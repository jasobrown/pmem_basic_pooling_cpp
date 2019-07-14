#include <fcntl.h>
#include <fstream>
#include <iostream>

#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_array.hpp>
#include <libpmemobj++/transaction.hpp>

#include "main.hpp"
#include "data_io.hpp"

using namespace pmem::obj;
using namespace jeb_pmem;

#define LAYOUT_NAME "jasobrown_layout"

/*
  PoolRoot functions
*/
size_t PoolRoot::GetUserRootOffset() {
    return userRootOffset;
}

void PoolRoot::SetUserRootOffset(size_t size) {

}

size_t PoolRoot::GetHeapSize() {
    return heapSize;
}

/*
  PoolWrapper functions
*/
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
    // TODO(jeb) fix this
    // if (pool_ != nullptr) {
    //     pool_.close();
    // }
}

/*
  general functionality
*/
std::shared_ptr<PoolWrapper<PoolRoot>> createPool(const std::string &path, size_t size) {
    auto pop = pool<PoolRoot>::create(path, LAYOUT_NAME, size);
    auto root = pop.root();

    transaction::run(pop, [&] {
        root->heapSize = PMEMOBJ_MIN_POOL;
        pop.persist(root->heapSize);
        root->userRootOffset = 42*100000;
        pop.persist(root->userRootOffset);
    });
    return std::make_shared<PoolWrapper<PoolRoot>>(pop);
}

std::shared_ptr<PoolWrapper<PoolRoot>> loadPool(const std::string &path, size_t size) {
    auto pop = pool<PoolRoot>::open(path, LAYOUT_NAME);
    // read existing root object
    // assert that stored size == size argument
    return std::make_shared<PoolWrapper<PoolRoot>>(pop);
}

void dumpHex(const unsigned char &data, int length) {
    //    std::cout << data <<std::endl;
    //    for (int i = 0; i < length; ++i) {
        std::cout << std::hex << data << std::endl;
        // }
}

int main() {

    // *1* create an array of pools, one per each NUMA node
    
    // do a check to see if file exists, instead of blindly creating
    auto filename = "/mnt/mem/pmem-pooling-example";
    std::ifstream poolCheck(filename);

    std::shared_ptr<PoolWrapper<PoolRoot>> pool;
    if (poolCheck) {
        pool = loadPool(filename, PMEMOBJ_MIN_POOL);
    } else {
        pool = createPool(filename, PMEMOBJ_MIN_POOL);
        // now, set (or read) the user root object 
        //   allocate the block, update it, and commit
        // then, update the root block to point to this
    }

    // size will, of course need to be determined at each call site
    //persistent_ptr<unsigned char[]> buf;
    int size = 1024;
    transaction::run(pool->pool_, [&] {
       auto buf = make_persistent<unsigned char[]>(size);
       MemBuffer membuf(buf, size);
       int32_t v = -42;
       membuf.write(v);

       int size = 256;
       unsigned char arr[size];
       for (int i = 0; i < size; ++i) {
           arr[i] = i;
       }
       membuf.write(arr, size);
       
       // bonus points fpor linking this in the user root obj!!!!
       // not the fucking p_ptr, but the raw addr .... actually, p_ptr
       // might be legit .....
    });
    
    // *2* now create a pool of threads, with affinity set per CPU
    // (related to NUMA node)
    
    return 0;
}
