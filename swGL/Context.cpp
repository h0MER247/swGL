#include <memory>
#include "Defines.h"
#include "MatrixStack.h"
#include "VertexPipeline.h"
#include "Context.h"

namespace SWGL {

    std::map<int, ContextPtr> Context::m_contextMap;
    ContextPtr Context::m_currentContext = nullptr;



    Context::Context(ContextID id, HDC hdc)
    
        : m_id(id),
          m_isCurrent(false),
          m_glExtensions("") {

        // Initialize drawing surface with the given device context
        auto &drawSurface = m_renderer.getDrawSurface();
        drawSurface.setHDC(hdc);

        // Initialize the dimenions of the viewport and scissor rect to the dimensions
        // of the device context
        m_vertexPipeline.getViewport().setDimensions(0, 0, drawSurface.getWidth(), drawSurface.getHeight());
        m_scissor.setDimensions(0, 0, drawSurface.getWidth(), drawSurface.getHeight());


        // Register Open GL 1.2 procedures
        addProcedure("glDrawRangeElements", ADDRESS_OF(glDrv_glDrawRangeElements));
        addProcedure("glTexImage3D", ADDRESS_OF(glDrv_glTexImage3D));
        addProcedure("glTexSubImage3D", ADDRESS_OF(glDrv_glTexSubImage3D));
        addProcedure("glCopyTexSubImage3D", ADDRESS_OF(glDrv_glCopyTexSubImage3D));

        // Register Open GL 1.3 procedures
        addProcedure("glActiveTexture", ADDRESS_OF(glDrv_glActiveTexture));
        addProcedure("glSampleCoverage", ADDRESS_OF(glDrv_glSampleCoverage));
        addProcedure("glCompressedTexImage3D", ADDRESS_OF(glDrv_glCompressedTexImage3D));
        addProcedure("glCompressedTexImage2D", ADDRESS_OF(glDrv_glCompressedTexImage2D));
        addProcedure("glCompressedTexImage1D", ADDRESS_OF(glDrv_glCompressedTexImage1D));
        addProcedure("glCompressedTexSubImage3D", ADDRESS_OF(glDrv_glCompressedTexSubImage3D));
        addProcedure("glCompressedTexSubImage2D", ADDRESS_OF(glDrv_glCompressedTexSubImage2D));
        addProcedure("glCompressedTexSubImage1D", ADDRESS_OF(glDrv_glCompressedTexSubImage1D));
        addProcedure("glGetCompressedTexImage", ADDRESS_OF(glDrv_glGetCompressedTexImage));
        addProcedure("glClientActiveTexture", ADDRESS_OF(glDrv_glClientActiveTexture));
        addProcedure("glMultiTexCoord1d", ADDRESS_OF(glDrv_glMultiTexCoord1d));
        addProcedure("glMultiTexCoord1dv", ADDRESS_OF(glDrv_glMultiTexCoord1dv));
        addProcedure("glMultiTexCoord1f", ADDRESS_OF(glDrv_glMultiTexCoord1f));
        addProcedure("glMultiTexCoord1fv", ADDRESS_OF(glDrv_glMultiTexCoord1fv));
        addProcedure("glMultiTexCoord1i", ADDRESS_OF(glDrv_glMultiTexCoord1i));
        addProcedure("glMultiTexCoord1iv", ADDRESS_OF(glDrv_glMultiTexCoord1iv));
        addProcedure("glMultiTexCoord1s", ADDRESS_OF(glDrv_glMultiTexCoord1s));
        addProcedure("glMultiTexCoord1sv", ADDRESS_OF(glDrv_glMultiTexCoord1sv));
        addProcedure("glMultiTexCoord2d", ADDRESS_OF(glDrv_glMultiTexCoord2d));
        addProcedure("glMultiTexCoord2dv", ADDRESS_OF(glDrv_glMultiTexCoord2dv));
        addProcedure("glMultiTexCoord2f", ADDRESS_OF(glDrv_glMultiTexCoord2f));
        addProcedure("glMultiTexCoord2fv", ADDRESS_OF(glDrv_glMultiTexCoord2fv));
        addProcedure("glMultiTexCoord2i", ADDRESS_OF(glDrv_glMultiTexCoord2i));
        addProcedure("glMultiTexCoord2iv", ADDRESS_OF(glDrv_glMultiTexCoord2iv));
        addProcedure("glMultiTexCoord2s", ADDRESS_OF(glDrv_glMultiTexCoord2s));
        addProcedure("glMultiTexCoord2sv", ADDRESS_OF(glDrv_glMultiTexCoord2sv));
        addProcedure("glMultiTexCoord3d", ADDRESS_OF(glDrv_glMultiTexCoord3d));
        addProcedure("glMultiTexCoord3dv", ADDRESS_OF(glDrv_glMultiTexCoord3dv));
        addProcedure("glMultiTexCoord3f", ADDRESS_OF(glDrv_glMultiTexCoord3f));
        addProcedure("glMultiTexCoord3fv", ADDRESS_OF(glDrv_glMultiTexCoord3fv));
        addProcedure("glMultiTexCoord3i", ADDRESS_OF(glDrv_glMultiTexCoord3i));
        addProcedure("glMultiTexCoord3iv", ADDRESS_OF(glDrv_glMultiTexCoord3iv));
        addProcedure("glMultiTexCoord3s", ADDRESS_OF(glDrv_glMultiTexCoord3s));
        addProcedure("glMultiTexCoord3sv", ADDRESS_OF(glDrv_glMultiTexCoord3sv));
        addProcedure("glMultiTexCoord4d", ADDRESS_OF(glDrv_glMultiTexCoord4d));
        addProcedure("glMultiTexCoord4dv", ADDRESS_OF(glDrv_glMultiTexCoord4dv));
        addProcedure("glMultiTexCoord4f", ADDRESS_OF(glDrv_glMultiTexCoord4f));
        addProcedure("glMultiTexCoord4fv", ADDRESS_OF(glDrv_glMultiTexCoord4fv));
        addProcedure("glMultiTexCoord4i", ADDRESS_OF(glDrv_glMultiTexCoord4i));
        addProcedure("glMultiTexCoord4iv", ADDRESS_OF(glDrv_glMultiTexCoord4iv));
        addProcedure("glMultiTexCoord4s", ADDRESS_OF(glDrv_glMultiTexCoord4s));
        addProcedure("glMultiTexCoord4sv", ADDRESS_OF(glDrv_glMultiTexCoord4sv));
        addProcedure("glLoadTransposeMatrixf", ADDRESS_OF(glDrv_glLoadTransposeMatrixf));
        addProcedure("glLoadTransposeMatrixd", ADDRESS_OF(glDrv_glLoadTransposeMatrixd));
        addProcedure("glMultTransposeMatrixf", ADDRESS_OF(glDrv_glMultTransposeMatrixf));
        addProcedure("glMultTransposeMatrixd", ADDRESS_OF(glDrv_glMultTransposeMatrixd));

        // Register the implemented extensions
        addExtension("GL_ARB_texture_env_add");
        addExtension("GL_ARB_texture_env_combine");
        addExtension("GL_ARB_texture_cube_map");
        addExtension("GL_ARB_texture_env_dot3");
        addExtension("GL_ARB_multitexture"); {

            addProcedure("glMultiTexCoord1dARB", ADDRESS_OF(glDrv_glMultiTexCoord1d));
            addProcedure("glMultiTexCoord1dvARB", ADDRESS_OF(glDrv_glMultiTexCoord1dv));
            addProcedure("glMultiTexCoord1fARB", ADDRESS_OF(glDrv_glMultiTexCoord1f));
            addProcedure("glMultiTexCoord1fvARB", ADDRESS_OF(glDrv_glMultiTexCoord1fv));
            addProcedure("glMultiTexCoord1iARB", ADDRESS_OF(glDrv_glMultiTexCoord1i));
            addProcedure("glMultiTexCoord1ivARB", ADDRESS_OF(glDrv_glMultiTexCoord1iv));
            addProcedure("glMultiTexCoord1sARB", ADDRESS_OF(glDrv_glMultiTexCoord1s));
            addProcedure("glMultiTexCoord1svARB", ADDRESS_OF(glDrv_glMultiTexCoord1sv));
            addProcedure("glMultiTexCoord2dARB", ADDRESS_OF(glDrv_glMultiTexCoord2d));
            addProcedure("glMultiTexCoord2dvARB", ADDRESS_OF(glDrv_glMultiTexCoord2dv));
            addProcedure("glMultiTexCoord2fARB", ADDRESS_OF(glDrv_glMultiTexCoord2f));
            addProcedure("glMultiTexCoord2fvARB", ADDRESS_OF(glDrv_glMultiTexCoord2fv));
            addProcedure("glMultiTexCoord2iARB", ADDRESS_OF(glDrv_glMultiTexCoord2i));
            addProcedure("glMultiTexCoord2ivARB", ADDRESS_OF(glDrv_glMultiTexCoord2iv));
            addProcedure("glMultiTexCoord2sARB", ADDRESS_OF(glDrv_glMultiTexCoord2s));
            addProcedure("glMultiTexCoord2svARB", ADDRESS_OF(glDrv_glMultiTexCoord2sv));
            addProcedure("glMultiTexCoord3dARB", ADDRESS_OF(glDrv_glMultiTexCoord3d));
            addProcedure("glMultiTexCoord3dvARB", ADDRESS_OF(glDrv_glMultiTexCoord3dv));
            addProcedure("glMultiTexCoord3fARB", ADDRESS_OF(glDrv_glMultiTexCoord3f));
            addProcedure("glMultiTexCoord3fvARB", ADDRESS_OF(glDrv_glMultiTexCoord3fv));
            addProcedure("glMultiTexCoord3iARB", ADDRESS_OF(glDrv_glMultiTexCoord3i));
            addProcedure("glMultiTexCoord3ivARB", ADDRESS_OF(glDrv_glMultiTexCoord3iv));
            addProcedure("glMultiTexCoord3sARB", ADDRESS_OF(glDrv_glMultiTexCoord3s));
            addProcedure("glMultiTexCoord3svARB", ADDRESS_OF(glDrv_glMultiTexCoord3sv));
            addProcedure("glMultiTexCoord4dARB", ADDRESS_OF(glDrv_glMultiTexCoord4d));
            addProcedure("glMultiTexCoord4dvARB", ADDRESS_OF(glDrv_glMultiTexCoord4dv));
            addProcedure("glMultiTexCoord4fARB", ADDRESS_OF(glDrv_glMultiTexCoord4f));
            addProcedure("glMultiTexCoord4fvARB", ADDRESS_OF(glDrv_glMultiTexCoord4fv));
            addProcedure("glMultiTexCoord4iARB", ADDRESS_OF(glDrv_glMultiTexCoord4i));
            addProcedure("glMultiTexCoord4ivARB", ADDRESS_OF(glDrv_glMultiTexCoord4iv));
            addProcedure("glMultiTexCoord4sARB", ADDRESS_OF(glDrv_glMultiTexCoord4s));
            addProcedure("glMultiTexCoord4svARB", ADDRESS_OF(glDrv_glMultiTexCoord4sv));
            addProcedure("glActiveTextureARB", ADDRESS_OF(glDrv_glActiveTexture));
            addProcedure("glClientActiveTextureARB", ADDRESS_OF(glDrv_glClientActiveTexture));
        }
        addExtension("GL_EXT_compiled_vertex_array"); {

            addProcedure("glLockArraysEXT", ADDRESS_OF(glDrv_glLockArrays));
            addProcedure("glUnlockArraysEXT", ADDRESS_OF(glDrv_glUnlockArrays));
        }
        addExtension("WGL_3DFX_gamma_control"); {

            addProcedure("wglGetDeviceGammaRamp3DFX", ADDRESS_OF(glDrv_wglGetDeviceGammaRamp));
            addProcedure("wglSetDeviceGammaRamp3DFX", ADDRESS_OF(glDrv_wglSetDeviceGammaRamp));
        }
        addExtension("WGL_ARB_extensions_string"); {

            addProcedure("wglGetExtensionsStringARB", ADDRESS_OF(glDrv_wglGetExtensionsString));
        }
    }



    void Context::init() {

        m_renderer.init();
        m_isCurrent = true;
    }

    void Context::shutdown() {

        m_renderer.shutdown();
        m_isCurrent = false;
    }



    const char *Context::getLibExtensions() const {
    
        return m_glExtensions.c_str();
    }

    const void *Context::getLibProcedure(std::string name) const {

        auto proc = m_glProcedures.find(name);
        if (proc != m_glProcedures.end()) {

            return proc->second;
        }

        LOG("Procedure %s isn't implemented", name.c_str());

        return nullptr;
    }

    void Context::addProcedure(std::string name, const void *address) {

        m_glProcedures[name] = address;
    }

    void Context::addExtension(std::string extension) {

        if (!m_glExtensions.empty()) {

            m_glExtensions.append(" ");
        }
        m_glExtensions.append(extension);
    }



    ContextID Context::createContext(HDC hdc) {

        static SWGL::ContextID id(1U);

        if (id == 0) {
        
            id++;
        }
        m_contextMap[id] = std::make_shared<SWGL::Context>(id, hdc);

        return id++;
    }

    ContextPtr Context::getContext(SWGL::ContextID id) {

        auto context = m_contextMap.find(id);
        if (context != m_contextMap.end()) {

            return context->second;
        }
        return nullptr;
    }

    bool Context::deleteContext(SWGL::ContextID id) {

        auto context = getContext(id);
        if (context != nullptr) {

            if (context == m_currentContext) {

                m_currentContext->shutdown();
                m_currentContext = nullptr;
            }

            m_contextMap.erase(id);
            return true;
        }

        return false;
    }

    void Context::setCurrentContext(ContextPtr context, HDC hdc) {

        if (m_currentContext != nullptr &&
            m_currentContext != context) {

            m_currentContext->shutdown();
        }

        if (context != nullptr) {

            if (context != m_currentContext) {
                context->init();
                m_currentContext = context;
            }

            m_currentContext->getRenderer().getDrawSurface().setHDC(hdc);
        }
        else {

            m_currentContext = nullptr;
        }
    }

    const ContextPtr &Context::getCurrentContext() {

        return m_currentContext;
    }
}
