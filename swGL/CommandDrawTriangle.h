#pragma once

#include "CommandBase.h"

namespace SWGL {

    //
    // The state that is needed in order to rasterize and shade triangles
    //
    struct TriangleDrawCallState {

        TriangleList triangles;

        Scissor scissor;
        AlphaTesting alphaTesting;
        DepthTesting depthTesting;
        Blending blending;
        PolygonOffset polygonOffset;
        bool deferedDepthWrite;

        struct TextureState {

            TextureObjectPtr texObj;
            TextureParameter texParams;
            TextureEnvironment texEnv;

        } textures[SWGL_MAX_TEXTURE_UNITS];
    };

    using TriangleDrawCallStatePtr = std::shared_ptr<TriangleDrawCallState>;

    //
    // Draw triangle(s) command
    //
    class CommandDrawTriangle : public CommandBase {

    public:
        CommandDrawTriangle(TriangleDrawCallStatePtr state, std::vector<int> &&indices)

            : m_state(state),
              m_indices(std::move(indices)) {

        }
        ~CommandDrawTriangle() = default;

    public:
        int getWorkLoadEstimate() {

            // TODO: This really isn't a good estimation, but better than nothing :)
            return m_indices.size();
        }

        bool execute(DrawThread *thread) override;

    private:
        TriangleDrawCallStatePtr m_state;
        std::vector<int> m_indices;
    };
}
