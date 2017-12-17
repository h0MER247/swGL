#pragma once

#include <condition_variable>
#include <thread>
#include <mutex>
#include <memory>
#include "Defines.h"
#include "DrawBuffer.h"
#include "CommandBase.h"
#include "LockFreeQueue.h"

namespace SWGL {
    
    // Forward declarations
    class DrawThread;

    // Type aliases
    using DrawThreadPtr = std::unique_ptr<DrawThread>;

    //
    // Implements the drawing thread which executes commands from the renderer
    //
    class DrawThread {

    public:
        DrawThread(DrawBufferPtr drawBuffer);
        ~DrawThread() = default;

    public:
        void start();
        void join();

    private:
        void run();

    public:
        void addCommand(CommandPtr command);
        DrawBuffer &getDrawBuffer() { return *m_drawBuffer; }

    private:
        int m_workloadEstimate;
        bool m_isWorkAvailable;
        std::condition_variable m_workAvailable;
        std::mutex m_mutex;
        LockFreeQueue<CommandPtr, SWGL_COMMAND_QUEUE_SIZE> m_commandQueue;
        std::thread m_thread;

    private:
        DrawBufferPtr m_drawBuffer;
    };
}
