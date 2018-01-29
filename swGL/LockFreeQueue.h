#pragma once

#include <atomic>
#include <array>
#include <cstddef>

namespace SWGL {

    //
    // Implementation of a array based lock free queue
    //
    template<typename T, size_t size>
    class LockFreeQueue {

    public:
        LockFreeQueue()

            : m_head(0),
              m_tail(0) {

        }
        ~LockFreeQueue() = default;

    public:
        bool push(T &item) {

            const auto currentTail = m_tail.load(std::memory_order_relaxed);
            const auto nextTail = getNextIndex(currentTail);

            if (nextTail != m_head.load(std::memory_order_acquire)) {

                m_elements[currentTail] = std::move(item);
                m_tail.store(nextTail, std::memory_order_release);
                return true;
            }

            return false;
        }

        bool pop(T &item) {

            const auto currentHead = m_head.load(std::memory_order_relaxed);
            if (currentHead == m_tail.load(std::memory_order_acquire)) {

                return false;
            }

            item = std::move(m_elements[currentHead]);
            m_head.store(getNextIndex(currentHead), std::memory_order_release);
            return true;
        }

    private:
        static constexpr size_t CAPACITY = size + 1;
        static constexpr size_t getNextIndex(size_t idx) {
        
            return (idx + 1) % CAPACITY;
        }

    private:
        std::atomic<size_t> m_head;
        std::array<T, CAPACITY> m_elements;
        std::atomic<size_t> m_tail;
    };
}
