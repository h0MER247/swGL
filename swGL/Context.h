#pragma once

#include <Windows.h>
#include <memory>
#include <map>
#include "OpenGL.h"
#include "Wiggle.h"
#include "Renderer.h"
#include "VertexPipeline.h"
#include "TextureManager.h"
#include "ContextTypes.h"

namespace SWGL {

    // Forward declarations
    class Context;

    // Type aliases
    using ContextPtr = std::shared_ptr<Context>;
    using ContextID = unsigned int;

    //
    // The context is the root object of swGL
    //
    class Context {

    public:
        Context(ContextID id, HDC hdc);
        ~Context() = default;

    public:
        void init();
        void shutdown();

    public:
        static ContextID createContext(HDC hdc);
        static ContextPtr getContext(ContextID id);
        static bool deleteContext(ContextID id);
        static void setCurrentContext(ContextPtr context, HDC hdc);
        static const ContextPtr &getCurrentContext();

    public:
        ContextID getContextID() { return m_id; }
        Scissor &getScissor() { return m_scissor; }
        VertexPipeline &getVertexPipeline() { return m_vertexPipeline; }
        ClearValues &getClearValues() { return m_clearValues; }
        AlphaTesting &getAlphaTesting() { return m_alphaTesting; }
        DepthTesting &getDepthTesting() { return m_depthTesting; }
        Blending &getBlending() { return m_blending; }
        TextureManager &getTextureManager() { return m_textureManager; }
        PolygonOffset &getPolygonOffset() { return m_polygonOffset; }
        ColorMask &getColorMask() { return m_colorMask; }
        Renderer &getRenderer() { return m_renderer; }
        GLError &getError() { return m_error; }

    public:
        const char *getLibVendor() const { return "h0MER247"; }
        const char *getLibRenderer() const { return "swGL"; }
        const char *getLibVersion() const { return "1.3"; }
        const char *getLibExtensions() const;
        const void *getLibProcedure(std::string name) const;

    private:
        ContextID m_id;
        bool m_isCurrent;

    private:
        GLError m_error;

    private:
        Scissor m_scissor;
        VertexPipeline m_vertexPipeline;

    private:
        ClearValues m_clearValues;
        PolygonOffset m_polygonOffset;
        AlphaTesting m_alphaTesting;
        DepthTesting m_depthTesting;
        Blending m_blending;
        ColorMask m_colorMask;
        TextureManager m_textureManager;
        Renderer m_renderer;

    private:
        void addProcedure(std::string name, const void *address);
        void addExtension(std::string extension);
        std::map<std::string, const void *> m_glProcedures;
        std::string m_glExtensions;

    private:
        static std::map<int, ContextPtr> m_contextMap;
        static ContextPtr m_currentContext;
    };
}
