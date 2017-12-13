#include <algorithm>
#include <array>
#include <vector>
#include "Defines.h"
#include "Context.h"
#include "Log.h"
#include "CommandClearColor.h"
#include "CommandClearDepth.h"
#include "CommandClearStencil.h"
#include "CommandDrawTriangle.h"
#include "CommandPoisonPill.h"
#include "CommandSyncPoint.h"
#include "Renderer.h"

namespace SWGL {

    Renderer::Renderer() {

        m_drawThreads.resize(SWGL_NUM_DRAW_THREADS);
    }



    void Renderer::init() {

        for (int i = 0; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_drawThreads[i] = std::make_unique<DrawThread>(

                m_drawSurface.getBuffer(i)
                );
            m_drawThreads[i]->start();
        }
    }



    void Renderer::clearColorBuffer() {

        auto clearColor = Context::getCurrentContext()->getClearValues().getClearColor();

        for (int i = 0; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_drawThreads[i]->addCommand(

                std::make_unique<CommandClearColor>(clearColor)
            );
        }
    }

    void Renderer::clearDepthBuffer() {

        auto clearDepth = Context::getCurrentContext()->getClearValues().getClearDepth();

        for (int i = 0; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_drawThreads[i]->addCommand(

                std::make_unique<CommandClearDepth>(clearDepth)
            );
        }
    }

    void Renderer::clearStencilBuffer() {

        auto clearStencil = Context::getCurrentContext()->getClearValues().getClearStencil();

        for (int i = 0; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_drawThreads[i]->addCommand(

                std::make_unique<CommandClearStencil>(clearStencil)
            );
        }
    }

    void Renderer::drawTriangles(TriangleList &&triangles) {

        auto &context = *Context::getCurrentContext();
        auto &scissor = context.getScissor();
        auto &texManager = context.getTextureManager();


        // Create the data that is shared by different drawing threads
        // and is used to draw the triangles
        auto drawState = std::make_shared<TriangleDrawCallState>();

        drawState->triangles = std::move(triangles);
        drawState->alphaTesting = context.getAlphaTesting();
        drawState->depthTesting = context.getDepthTesting();
        drawState->blending = context.getBlending();
        drawState->polygonOffset = context.getPolygonOffset();
        drawState->deferedDepthWrite = context.getAlphaTesting().isEnabled() &&
                                       context.getDepthTesting().isWriteEnabled() &&
                                       context.getDepthTesting().isTestEnabled();

        for (size_t i = 0U; i < SWGL_MAX_TEXTURE_UNITS; i++) {

            TextureUnit &unit = texManager.getTextureUnit(i);

            if (unit.currentTarget != nullptr &&
                unit.currentTarget->isEnabled &&
                unit.currentTarget->texObj != nullptr) {

                drawState->textures[i].texEnv = unit.texEnv;
                drawState->textures[i].texObj = unit.currentTarget->texObj;
                drawState->textures[i].texParams = unit.currentTarget->texObj->parameter;
            }
            else {

                drawState->textures[i].texObj = nullptr;
            }
        }


        // Figure out which triangle must be rendered by which thread
        std::array<std::vector<int>, SWGL_NUM_DRAW_THREADS> bins;

        auto binWidth = m_drawSurface.getBufferWidth();
        auto binHeight = m_drawSurface.getBufferHeight();
        auto numBinsX = m_drawSurface.getNumBuffersInX();
        auto numBinsY = m_drawSurface.getNumBuffersInY();

        for (int i = 0, n = drawState->triangles.size(); i < n; i++) {

            auto &t = drawState->triangles[i];
            if (!t.isVisible) {

                continue;
            }

            auto &v1 = t.v[0].raster;
            auto &v2 = t.v[1].raster;
            auto &v3 = t.v[2].raster;

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
        for (int i = 0; i < SWGL_NUM_DRAW_THREADS; i++) {

            if (!bins[i].empty()) {

                m_drawThreads[i]->addCommand(

                    std::make_unique<CommandDrawTriangle>(

                        drawState, std::move(bins[i])
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

        for (int i = 0; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_drawThreads[i]->addCommand(

                std::make_unique<CommandPoisonPill>()
            );
        }

        for (int i = 0; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_drawThreads[i]->join();
        }
    }



    void Renderer::synchronize() {

        m_latch.reset(SWGL_NUM_DRAW_THREADS);

        for (int i = 0; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_drawThreads[i]->addCommand(

                std::make_unique<CommandSyncPoint>(m_latch)
            );
        }

        m_latch.wait();
    }
}
