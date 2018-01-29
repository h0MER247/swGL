#include <stdexcept>
#include "Log.h"
#include "DrawThread.h"

namespace SWGL {

    DrawThread::DrawThread(DrawBufferPtr drawBuffer)

        : m_drawBuffer(drawBuffer) {

    }



    void DrawThread::start() {

        m_workloadEstimate = 0;
        m_isWorkAvailable = false;

        m_thread = std::thread(&DrawThread::run, this);
    }

    void DrawThread::join() {

        try {

            if (m_thread.joinable()) {

                m_thread.join();
            }
        }
        catch (std::runtime_error ex) {

            LOG("std::thread::join() failed: %s", ex.what());
        }
    }



    void DrawThread::addCommand(CommandPtr command) {

        auto isFlushingQueue = command->isFlushingQueue();
        auto workloadEstimate = command->getWorkLoadEstimate();

        // This is used to not overburden or starve the threads with too many / too few triangles.
        // If it goes beyond a certain threshold the drawing process is forced to start. It's only
        // a hack until something more sophisticated will replace this - but it isn't "bad" as it
        // increases the FPS by a significant amount.
        m_workloadEstimate += workloadEstimate;

        for (;;) {
        
            bool couldPush = m_commandQueue.push(command);

            // Tell the draw thread that there is work to do
            if (!couldPush || m_workloadEstimate >= 32 || isFlushingQueue) {

                {
                    std::lock_guard<std::mutex> cs(m_mutex);
                    m_isWorkAvailable = true;
                }
                m_workAvailable.notify_one();

                if (!couldPush) { continue; }
            }

            if (couldPush) {

                if (m_workloadEstimate >= 32) {
                
                    m_workloadEstimate = 0;
                }
                break;
            }
        }
    }



    void DrawThread::run() {

        std::unique_lock<std::mutex> cs(m_mutex, std::defer_lock);

        CommandPtr cmd;
        for (;;) {

            // Wait for work
            cs.lock();
            m_workAvailable.wait(cs, [this] {

                return m_isWorkAvailable;
            });
            m_isWorkAvailable = false;
            cs.unlock();

            // Execute the commands
            while (m_commandQueue.pop(cmd)) {

                if (!cmd->execute(this)) {

                    return;
                }
            }
        }
    }
}
