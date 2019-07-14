#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>

/// A wrapper around a persistent_ptr<unsigned char[]> (basically, a
/// byte buffer)
/// that allows us to read and write primitive data types as well as
/// char arrays.
/// This is inspired by java's DataInput/DataOutput classes,
/// and the read/write indices borrowed from netty.
///
/// Assumes you (I) haven't fucked up and closed (or not opened) a transaction.
class MemBuffer {
  public:
    MemBuffer(persistent_ptr<unsigned char[]> buf, size_t size) : buf_(buf), size_(size), write_idx_(0), read_idx_(0) {};

    // TODO(jeb) firgure out other ctors, assignment ops

    // doesn't work with arrays ...
    template<typename T>
    bool write(T t) {
        size_t s = sizeof(T);
        if (s > size_ - write_idx_) {
            return false;
        }

        // this copies out as little-endian, do i care?
        std::memcpy(buf_.get() + write_idx_, &t, s);
        write_idx_ += s;
        return true;
    }
    
    template<typename T>
    bool write(T t[], size_t len) {
        if (len > size_ - write_idx_) {
            return false;
        }

        // this copies out as little-endian, do i care?
        std::memcpy(buf_.get() + write_idx_, t, len);
        write_idx_ += len;
        return true;
    }
    
  private:
    persistent_ptr<unsigned char[]> buf_;
    size_t size_;
    size_t write_idx_;
    size_t read_idx_;
};
