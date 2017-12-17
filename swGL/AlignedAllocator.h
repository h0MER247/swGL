#pragma once

#include <malloc.h>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include "Defines.h"

namespace SWGL {

    //
    // Allocator for aligned memory
    //
    template<typename T, size_t alignment>
    class AlignedAllocator {

    public:
        AlignedAllocator() = default;
        ~AlignedAllocator() = default;

    public:
        typedef T * pointer;
        typedef const T * const_pointer;
        typedef T & reference;
        typedef const T & const_reference;
        typedef T value_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    public:
        template<class U>
        struct rebind {

            typedef AlignedAllocator<U, alignment> other;
        };
        template<class U>
        AlignedAllocator(AlignedAllocator<U, alignment> const &) {
        };

    public:
        T *address(T &r) const { return &r; }
        const T *address(const T &s) const { return &s; }

    public:
        size_t max_size() const {

            return std::numeric_limits<size_t>::max() / sizeof(T);
        }

    public:
        bool operator==(const AlignedAllocator& rhs) { return true; }
        bool operator!=(const AlignedAllocator& rhs) { return false; }

    public:
        void construct(T * const p, const T &v) const {

            new(static_cast<void *>(p)) T(v);
        }

        void destroy(T * const p) const {

            p->~T();
        }

        T *allocate(const size_t n) const {

            if (n == 0) {

                return nullptr;
            }
            else if (n > max_size()) {

                throw std::length_error("AlignedAllocator<T>::allocate(): Too many elements.");
            }
            else {

                auto pv = _mm_malloc(n * sizeof(T), alignment);
                if (pv == nullptr) {

                    throw std::bad_alloc();
                }
                return static_cast<T *>(pv);
            }
        }

        void deallocate(T * const p, const size_t n) const {

            _mm_free(p);
        }
    };
}
