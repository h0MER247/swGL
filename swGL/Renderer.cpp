#include <array>
#include <vector>
#include "Defines.h"
#include "Context.h"
#include "Log.h"
#include "CommandClearColor.h"
#include "CommandClearDepth.h"
#include "CommandDrawTriangle.h"
#include "CommandPoisonPill.h"
#include "CommandSynchronize.h"
#include "Renderer.h"

namespace SWGL {

    Renderer::Renderer() {

        m_drawThreads.resize(SWGL_NUM_DRAW_THREADS);
    }



    void Renderer::init() {
        
        for (auto i = 0U; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_drawThreads[i] = std::make_unique<DrawThread>(

                m_drawSurface.getBuffer(i)
            );
            m_drawThreads[i]->start();
        }
    }



    void Renderer::clearColorBuffer() {

        auto &ctx = Context::getCurrentContext();

        auto &scissor = ctx->getScissor();
        auto clearColor = ctx->getClearValues().getClearColor();

        for (auto i = 0U; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_drawThreads[i]->addCommand(

                std::make_unique<CommandClearColor>(

                    clearColor,
                    scissor.getMinX(),
                    scissor.getMinY(),
                    scissor.getMaxX(),
                    scissor.getMaxY()
                )
            );
        }
    }

    void Renderer::clearDepthBuffer() {

        auto &ctx = Context::getCurrentContext();

        auto &scissor = ctx->getScissor();
        auto clearDepth = ctx->getClearValues().getClearDepth();

        for (auto i = 0U; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_drawThreads[i]->addCommand(

                std::make_unique<CommandClearDepth>(

                    clearDepth,
                    scissor.getMinX(),
                    scissor.getMinY(),
                    scissor.getMaxX(),
                    scissor.getMaxY()
                 )
            );
        }
    }

    void Renderer::drawTriangles(TriangleList &triangles) {

        auto &context = *Context::getCurrentContext();
        auto &scissor = context.getScissor();
        auto &texManager = context.getTextureManager();


        // Create the data that is shared by different drawing threads
        // and is used to draw the triangles
        auto drawState = std::make_shared<TriangleDrawCallState>();

        drawState->triangles = std::move(triangles);
        drawState->scissor = scissor;
        drawState->polygonOffset = context.getPolygonOffset();
        drawState->depthTesting = context.getDepthTesting();
        drawState->alphaTesting = context.getAlphaTesting();
        drawState->blending = context.getBlending();
        drawState->colorMask = context.getColorMask();
        drawState->deferedDepthWrite = context.getAlphaTesting().isEnabled() &&
                                       context.getDepthTesting().isWriteEnabled() &&
                                       context.getDepthTesting().isTestEnabled();

        for (auto i = 0U; i < SWGL_MAX_TEXTURE_UNITS; i++) {

            auto &unit = texManager.getTextureUnit(i);
            auto &texState = drawState->textures[i];

            if (unit.currentTarget != nullptr) {

                auto &texObj = unit.currentTarget->texObj;

                // TODO: Implement a flag for texture completeness (texObj->isComplete or
                //       something like that)
                if (texObj != nullptr &&
                    texObj->data != nullptr &&
                    texObj->data->maxLOD > -1) {

                    texState.texEnv = unit.texEnv;
                    texState.texData = texObj->data;
                    texState.texParams = texObj->parameter;
                    continue;
                }
            }

            texState.texData = nullptr;
        }


        // Figure out which triangle must be rendered by which thread
        std::array<std::vector<int>, SWGL_NUM_DRAW_THREADS> bins;

        auto binWidth = m_drawSurface.getBufferWidth();
        auto binHeight = m_drawSurface.getBufferHeight();
        auto numBinsX = m_drawSurface.getNumBuffersInX();
        auto numBinsY = m_drawSurface.getNumBuffersInY();

        for (auto i = 0u, n = drawState->triangles.size(); i < n; i++) {

            auto &t = drawState->triangles[i];

            auto &v1 = t.v[0].posObj;
            auto &v2 = t.v[1].posObj;
            auto &v3 = t.v[2].posObj;

            // Determine the triangles bounding box
            float y1 = v1.y(), y2 = v2.y(), y3 = v3.y();
            float x1 = v1.x(), x2 = v2.x(), x3 = v3.x();

            int minY = (static_cast<int>(16.0f * std::min({ y1, y2, y3 })) + 0x0f) >> 4;
            int maxY = (static_cast<int>(16.0f * std::max({ y1, y2, y3 })) + 0x0f) >> 4;
            int minX = (static_cast<int>(16.0f * std::min({ x1, x2, x3 })) + 0x0f) >> 4;
            int maxX = (static_cast<int>(16.0f * std::max({ x1, x2, x3 })) + 0x0f) >> 4;

            // Perform scissoring
            if (scissor.isEnabled()) {

                scissor.cut(minX, minY, maxX, maxY);
            }

            // Don't bother to bin triangles that have a zero sized bounding box
            if (minX >= maxX || minY >= maxY) {

                continue;
            }

            //
            // Add the triangle into the corresponding bin(s)
            //
            int binStartY = std::max(minY / binHeight, 0);
            int binEndY = std::min((maxY + binHeight - 1) / binHeight, numBinsY);
            int binStartX = std::max(minX / binWidth, 0);
            int binEndX = std::min((maxX + binWidth - 1) / binWidth, numBinsX);

            for (int y = binStartY; y < binEndY; y++) {

                int idxStart = binStartX + (y * numBinsX);
                int idxEnd = binEndX + (y * numBinsX);

                for (int idx = idxStart; idx < idxEnd; idx++) {

                    bins[idx].emplace_back(i);
                }
            }
        }

        // Add draw command
        for (auto i = 0U; i < SWGL_NUM_DRAW_THREADS; i++) {

            if (!bins[i].empty()) {

                m_drawThreads[i]->addCommand(

                    std::make_unique<CommandDrawTriangle>(

                        drawState, bins[i]
                    )
                );
            }
        }
    }

    void Renderer::finish() {

        synchronize();
    }

    void Renderer::swapBuffers() {

        synchronize();
        m_drawSurface.swap();
    }

    void Renderer::shutdown() {

        for (auto i = 0U; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_drawThreads[i]->addCommand(

                std::make_unique<CommandPoisonPill>()
            );
        }

        for (auto i = 0U; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_drawThreads[i]->join();
        }
    }



    void Renderer::synchronize() {

        m_latch.reset(SWGL_NUM_DRAW_THREADS);

        for (auto i = 0U; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_drawThreads[i]->addCommand(

                std::make_unique<CommandSynchronize>(m_latch)
            );
        }

        m_latch.wait();
    }
}
