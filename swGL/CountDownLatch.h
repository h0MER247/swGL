#pragma once

#include <mutex>
#include <atomic>
#include <condition_variable>

namespace SWGL {

    //
    // Implementation of a simple count down latch
    //
    class CountDownLatch {

    public:
        CountDownLatch()
        
            : m_count(0) {

        }
        ~CountDownLatch() = default;

    public:
        void reset(int count) {

            m_count = count;
        }

        void countDown() {

            std::lock_guard<std::mutex> cs(m_mutex);
            m_count--;
            if (m_count == 0) {

                m_condition.notify_one();
            }
        }

        void wait() {

            std::unique_lock<std::mutex> cs(m_mutex);
            m_condition.wait(cs, [this] {
            
                return m_count == 0;
            });
        }

    private:
        std::mutex m_mutex;
        std::atomic<int> m_count;
        std::condition_variable m_condition;
    };
}
