#include <algorithm>
#include "OpenGL.h"
#include "Context.h"
#include "Log.h"
#include "Matrix.h"
#include "Vector.h"
#include "VertexPipeline.h"
#include "MatrixStack.h"
#include "TextureManager.h"

#pragma region OpenGL 1.0

SWGLAPI void STDCALL glDrv_glAccum(GLenum op, GLfloat value) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glAlphaFunc(GLenum func, GLclampf ref) {

    LOG("Alpha Func: %04X, Ref: %f", func, ref);

    switch (func) {

    case GL_NEVER:
    case GL_LESS:
    case GL_EQUAL:
    case GL_LEQUAL:
    case GL_GREATER:
    case GL_NOTEQUAL:
    case GL_GEQUAL:
    case GL_ALWAYS:
        SWGL::Context::getCurrentContext()->getAlphaTesting().setAlphaFunc(func, ref);
        break;

    default:
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        return;
    }
}

SWGLAPI void STDCALL glDrv_glBegin(GLenum mode) {

    LOG("Mode: %04x", mode);

    switch (mode) {

    case GL_POINTS:
    case GL_LINES:
    case GL_LINE_STRIP:
    case GL_LINE_LOOP:
    case GL_TRIANGLES:
    case GL_TRIANGLE_STRIP:
    case GL_TRIANGLE_FAN:
    case GL_QUADS:
    case GL_QUAD_STRIP:
    case GL_POLYGON:
        break;

    default:
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        return;
    }

    SWGL::Context::getCurrentContext()->getVertexPipeline().begin(mode);
}

SWGLAPI void STDCALL glDrv_glBitmap(GLsizei width, GLsizei height, GLfloat xOrig, GLfloat yOrig, GLfloat xMove, GLfloat yMove, const GLubyte *bitmap) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glBlendFunc(GLenum srcFactor, GLenum dstFactor) {

    LOG("Source Factor: %04X, Destination Factor: %04X", srcFactor, dstFactor);

    // Validate the source factor
    switch (srcFactor) {

    case GL_ZERO:
    case GL_ONE:
    case GL_SRC_COLOR:
    case GL_ONE_MINUS_SRC_COLOR:
    case GL_DST_COLOR:
    case GL_ONE_MINUS_DST_COLOR:
    case GL_SRC_ALPHA:
    case GL_ONE_MINUS_SRC_ALPHA:
    case GL_DST_ALPHA:
    case GL_ONE_MINUS_DST_ALPHA:
    case GL_SRC_ALPHA_SATURATE:
        break;

    default:
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        return;
    }

    // Validate the destination factor
    switch (dstFactor) {

    case GL_ZERO:
    case GL_ONE:
    case GL_SRC_COLOR:
    case GL_ONE_MINUS_SRC_COLOR:
    case GL_DST_COLOR:
    case GL_ONE_MINUS_DST_COLOR:
    case GL_SRC_ALPHA:
    case GL_ONE_MINUS_SRC_ALPHA:
    case GL_DST_ALPHA:
    case GL_ONE_MINUS_DST_ALPHA:
        break;

    default:
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        return;
    }

    SWGL::Context::getCurrentContext()->getBlending().setFactors(srcFactor, dstFactor);
}

SWGLAPI void STDCALL glDrv_glCallList(GLuint list) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glCallLists(GLsizei n, GLenum type, const GLvoid *lists) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glClear(GLbitfield mask) {

    LOG("Mask: %04x", mask);

    auto &context = SWGL::Context::getCurrentContext();

    if ((mask & GL_COLOR_BUFFER_BIT) != 0)
        context->getRenderer().clearColorBuffer();

    if ((mask & GL_DEPTH_BUFFER_BIT) != 0)
        context->getRenderer().clearDepthBuffer();
}

SWGLAPI void STDCALL glDrv_glClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {

    LOG("Alpha = %f, Red = %f, Green = %f, Blue = %f", alpha, red, green, blue);

    int r = static_cast<int>(255.0f * std::clamp(red, 0.0f, 1.0f));
    int g = static_cast<int>(255.0f * std::clamp(green, 0.0f, 1.0f));
    int b = static_cast<int>(255.0f * std::clamp(blue, 0.0f, 1.0f));
    int a = static_cast<int>(255.0f * std::clamp(alpha, 0.0f, 1.0f));

    SWGL::Context::getCurrentContext()->getClearValues().setClearColor(
    
        (a << 24) | (r << 16) | (g << 8) | b
    );
}

SWGLAPI void STDCALL glDrv_glClearDepth(GLclampd depth) {

    LOG("Depth = %f", depth);

    depth = std::max(std::min(depth, 1.0), 0.0);

    SWGL::Context::getCurrentContext()->getClearValues().setClearDepth(
    
        static_cast<float>(depth)
    );
}

SWGLAPI void STDCALL glDrv_glClearIndex(GLfloat c) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glClearStencil(GLint s) {

    LOG("Stencil = %d", s);

    s &= 0xff;

    SWGL::Context::getCurrentContext()->getClearValues().setClearStencil(

        static_cast<unsigned char>(s)
    );
}

SWGLAPI void STDCALL glDrv_glClipPlane(GLenum plane, const GLdouble *equation) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glColor3b(GLbyte red, GLbyte green, GLbyte blue) {

    LOG("Red: %d, Green: %d, Blue: %d", red, green, blue);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(red),
            SWGL::Vector::normalizeInteger(green),
            SWGL::Vector::normalizeInteger(blue),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor3bv(const GLbyte *v) {

    LOG("Red: %d, Green: %d, Blue: %d", v[0], v[1], v[2]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(v[0]),
            SWGL::Vector::normalizeInteger(v[1]),
            SWGL::Vector::normalizeInteger(v[2]),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor3d(GLdouble red, GLdouble green, GLdouble blue) {

    LOG("Red: %f, Green: %f, Blue: %f", red, green, blue);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            static_cast<float>(red),
            static_cast<float>(green),
            static_cast<float>(blue),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor3dv(const GLdouble *v) {

    LOG("Red: %f, Green: %f, Blue: %f", v[0], v[1], v[2]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor3f(GLfloat red, GLfloat green, GLfloat blue) {

    LOG("Red: %f, Green: %f, Blue: %f", red, green, blue);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(red, green, blue, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glColor3fv(const GLfloat *v) {

    LOG("Red: %f, Green: %f, Blue: %f", v[0], v[1], v[2]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(v[0], v[1], v[2], 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glColor3i(GLint red, GLint green, GLint blue) {

    LOG("Red: %d, Green: %d, Blue: %d", red, green, blue);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(red),
            SWGL::Vector::normalizeInteger(green),
            SWGL::Vector::normalizeInteger(blue),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor3iv(const GLint *v) {

    LOG("Red: %d, Green: %d, Blue: %d", v[0], v[1], v[2]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(v[0]),
            SWGL::Vector::normalizeInteger(v[1]),
            SWGL::Vector::normalizeInteger(v[2]),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor3s(GLshort red, GLshort green, GLshort blue) {

    LOG("Red: %d, Green: %d, Blue: %d", red, green, blue);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(red),
            SWGL::Vector::normalizeInteger(green),
            SWGL::Vector::normalizeInteger(blue),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor3sv(const GLshort *v) {

    LOG("Red: %d, Green: %d, Blue: %d", v[0], v[1], v[2]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(v[0]),
            SWGL::Vector::normalizeInteger(v[1]),
            SWGL::Vector::normalizeInteger(v[2]),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor3ub(GLubyte red, GLubyte green, GLubyte blue) {

    LOG("Red: %d, Green: %d, Blue: %d", red, green, blue);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(red),
            SWGL::Vector::normalizeInteger(green),
            SWGL::Vector::normalizeInteger(blue),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor3ubv(const GLubyte *v) {

    LOG("Red: %d, Green: %d, Blue: %d", v[0], v[1], v[2]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(v[0]),
            SWGL::Vector::normalizeInteger(v[1]),
            SWGL::Vector::normalizeInteger(v[2]),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor3ui(GLuint red, GLuint green, GLuint blue) {

    LOG("Red: %d, Green: %d, Blue: %d", red, green, blue);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(red),
            SWGL::Vector::normalizeInteger(green),
            SWGL::Vector::normalizeInteger(blue),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor3uiv(const GLuint *v) {

    LOG("Red: %d, Green: %d, Blue: %d", v[0], v[1], v[2]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(v[0]),
            SWGL::Vector::normalizeInteger(v[1]),
            SWGL::Vector::normalizeInteger(v[2]),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor3us(GLushort red, GLushort green, GLushort blue) {

    LOG("Red: %d, Green: %d, Blue: %d", red, green, blue);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(red),
            SWGL::Vector::normalizeInteger(green),
            SWGL::Vector::normalizeInteger(blue),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor3usv(const GLushort *v) {

    LOG("Red: %d, Green: %d, Blue: %d", v[0], v[1], v[2]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(v[0]),
            SWGL::Vector::normalizeInteger(v[1]),
            SWGL::Vector::normalizeInteger(v[2]),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor4b(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha) {

    LOG("Red: %d, Green: %d, Blue: %d, Alpha: %d", red, green, blue, alpha);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(red),
            SWGL::Vector::normalizeInteger(green),
            SWGL::Vector::normalizeInteger(blue),
            SWGL::Vector::normalizeInteger(alpha)
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor4bv(const GLbyte *v) {

    LOG("Red: %d, Green: %d, Blue: %d, Alpha: %d", v[0], v[1], v[2], v[3]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(v[0]),
            SWGL::Vector::normalizeInteger(v[1]),
            SWGL::Vector::normalizeInteger(v[2]),
            SWGL::Vector::normalizeInteger(v[3])
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha) {

    LOG("Red: %f, Green: %f, Blue: %f, Alpha: %f", red, green, blue, alpha);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            static_cast<float>(red),
            static_cast<float>(green),
            static_cast<float>(blue),
            static_cast<float>(alpha)
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor4dv(const GLdouble *v) {

    LOG("Red: %f, Green: %f, Blue: %f, Alpha: %f", v[0], v[1], v[2], v[3]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            static_cast<float>(v[3])
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {

    LOG("Red: %f, Green: %f, Blue: %f, Alpha: %f", red, green, blue, alpha);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(red, green, blue, alpha)
    );
}

SWGLAPI void STDCALL glDrv_glColor4fv(const GLfloat *v) {

    LOG("Red: %f, Green: %f, Blue: %f, Alpha: %f", v[0], v[1], v[2], v[3]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(v[0], v[1], v[2], v[3])
    );
}

SWGLAPI void STDCALL glDrv_glColor4i(GLint red, GLint green, GLint blue, GLint alpha) {

    LOG("Red: %d, Green: %d, Blue: %d, Alpha: %d", red, green, blue, alpha);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(red),
            SWGL::Vector::normalizeInteger(green),
            SWGL::Vector::normalizeInteger(blue),
            SWGL::Vector::normalizeInteger(alpha)
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor4iv(const GLint *v) {

    LOG("Red: %d, Green: %d, Blue: %d, Alpha: %d", v[0], v[1], v[2], v[3]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(v[0]),
            SWGL::Vector::normalizeInteger(v[1]),
            SWGL::Vector::normalizeInteger(v[2]),
            SWGL::Vector::normalizeInteger(v[3])
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor4s(GLshort red, GLshort green, GLshort blue, GLshort alpha) {

    LOG("Red: %d, Green: %d, Blue: %d, Alpha: %d", red, green, blue, alpha);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(red),
            SWGL::Vector::normalizeInteger(green),
            SWGL::Vector::normalizeInteger(blue),
            SWGL::Vector::normalizeInteger(alpha)
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor4sv(const GLshort *v) {

    LOG("Red: %d, Green: %d, Blue: %d, Alpha: %d", v[0], v[1], v[2], v[3]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(v[0]),
            SWGL::Vector::normalizeInteger(v[1]),
            SWGL::Vector::normalizeInteger(v[2]),
            SWGL::Vector::normalizeInteger(v[3])
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha) {

    LOG("Red: %d, Green: %d, Blue: %d, Alpha: %d", red, green, blue, alpha);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(red),
            SWGL::Vector::normalizeInteger(green),
            SWGL::Vector::normalizeInteger(blue),
            SWGL::Vector::normalizeInteger(alpha)
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor4ubv(const GLubyte *v) {

    LOG("Red: %d, Green: %d, Blue: %d, Alpha: %d", v[0], v[1], v[2], v[3]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(v[0]),
            SWGL::Vector::normalizeInteger(v[1]),
            SWGL::Vector::normalizeInteger(v[2]),
            SWGL::Vector::normalizeInteger(v[3])
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor4ui(GLuint red, GLuint green, GLuint blue, GLuint alpha) {

    LOG("Red: %d, Green: %d, Blue: %d, Alpha: %d", red, green, blue, alpha);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(red),
            SWGL::Vector::normalizeInteger(green),
            SWGL::Vector::normalizeInteger(blue),
            SWGL::Vector::normalizeInteger(alpha)
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor4uiv(const GLuint *v) {

    LOG("Red: %d, Green: %d, Blue: %d, Alpha: %d", v[0], v[1], v[2], v[3]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(v[0]),
            SWGL::Vector::normalizeInteger(v[1]),
            SWGL::Vector::normalizeInteger(v[2]),
            SWGL::Vector::normalizeInteger(v[3])
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor4us(GLushort red, GLushort green, GLushort blue, GLushort alpha) {

    LOG("Red: %d, Green: %d, Blue: %d, Alpha: %d", red, green, blue, alpha);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(red),
            SWGL::Vector::normalizeInteger(green),
            SWGL::Vector::normalizeInteger(blue),
            SWGL::Vector::normalizeInteger(alpha)
        )
    );
}

SWGLAPI void STDCALL glDrv_glColor4usv(const GLushort *v) {

    LOG("Red: %d, Green: %d, Blue: %d, Alpha: %d", v[0], v[1], v[2], v[3]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setColor(

        SWGL::Vector(

            SWGL::Vector::normalizeInteger(v[0]),
            SWGL::Vector::normalizeInteger(v[1]),
            SWGL::Vector::normalizeInteger(v[2]),
            SWGL::Vector::normalizeInteger(v[3])
        )
    );
}

SWGLAPI void STDCALL glDrv_glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glColorMaterial(GLenum face, GLenum mode) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glCullFace(GLenum mode) {

    LOG("Mode: %04x", mode);

    switch (mode) {

    case GL_FRONT:
    case GL_BACK:
        SWGL::Context::getCurrentContext()->getCulling().setCullFace(mode);
        break;

    default:
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glDeleteLists(GLuint list, GLsizei range) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glDepthFunc(GLenum func) {

    LOG("Function: %04x", func);

    switch (func) {

    case GL_NEVER:
    case GL_LESS:
    case GL_EQUAL:
    case GL_LEQUAL:
    case GL_GREATER:
    case GL_NOTEQUAL:
    case GL_GEQUAL:
    case GL_ALWAYS:
        SWGL::Context::getCurrentContext()->getDepthTesting().setTestFunction(func);
        break;

    default:
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glDepthMask(GLboolean flag) {

    LOG("Depth Write Enable: %d", flag == GL_TRUE);

    SWGL::Context::getCurrentContext()->getDepthTesting().setWriteEnable(flag == GL_TRUE);
}

SWGLAPI void STDCALL glDrv_glDepthRange(GLclampd zNear, GLclampd zFar) {

    zNear = std::max(std::min(zNear, 1.0), 0.0);
    zFar = std::max(std::min(zFar, 1.0), 0.0);

    LOG("Near: %f, Far: %f", zNear, zFar);

    SWGL::Context::getCurrentContext()->getViewport().setDepthRange(

        static_cast<float>(zNear),
        static_cast<float>(zFar)
    );
}

SWGLAPI void STDCALL glDrv_glDisable(GLenum cap) {

    LOG("Capability: %04x", cap);

    switch (cap) {

    case GL_CULL_FACE:
        SWGL::Context::getCurrentContext()->getCulling().setEnable(false);
        break;

    case GL_DEPTH_TEST:
        SWGL::Context::getCurrentContext()->getDepthTesting().setTestEnable(false);
        break;

    case GL_BLEND:
        SWGL::Context::getCurrentContext()->getBlending().setEnable(false);
        break;

    case GL_ALPHA_TEST:
        SWGL::Context::getCurrentContext()->getAlphaTesting().setEnable(false);
        break;

    case GL_TEXTURE_2D:
        SWGL::Context::getCurrentContext()->getTextureManager().setTargetEnable(SWGL::TextureTargetID::Target2D, false);
        break;

    case GL_SCISSOR_TEST:
        SWGL::Context::getCurrentContext()->getScissor().setEnable(false);
        break;

    case GL_POLYGON_OFFSET_FILL:
        SWGL::Context::getCurrentContext()->getPolygonOffset().setFillEnable(false);
        break;

    default:
        LOG("Unimplemented");
        break;
    }
}

SWGLAPI void STDCALL glDrv_glDrawBuffer(GLenum mode) {

    LOG("Mode: %04x", mode);

    switch (mode) {

    case GL_BACK:
        break;

    case GL_FRONT:
        break;

    default:
        LOG("Unimplemented");
        break;
    }
}

SWGLAPI void STDCALL glDrv_glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEdgeFlag(GLboolean flag) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEdgeFlagv(const GLboolean *flag) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEnable(GLenum cap) {

    LOG("Capability: %04x", cap);

    switch (cap) {

    case GL_CULL_FACE:
        SWGL::Context::getCurrentContext()->getCulling().setEnable(true);
        break;

    case GL_DEPTH_TEST:
        SWGL::Context::getCurrentContext()->getDepthTesting().setTestEnable(true);
        break;

    case GL_BLEND:
        SWGL::Context::getCurrentContext()->getBlending().setEnable(true);
        break;

    case GL_ALPHA_TEST:
        SWGL::Context::getCurrentContext()->getAlphaTesting().setEnable(true);
        break;

    case GL_TEXTURE_2D:
        SWGL::Context::getCurrentContext()->getTextureManager().setTargetEnable(SWGL::TextureTargetID::Target2D, true);
        break;

    case GL_SCISSOR_TEST:
        SWGL::Context::getCurrentContext()->getScissor().setEnable(true);
        break;

    case GL_POLYGON_OFFSET_FILL:
        SWGL::Context::getCurrentContext()->getPolygonOffset().setFillEnable(true);
        break;

    default:
        LOG("Unimplemented");
        break;
    }
}

SWGLAPI void STDCALL glDrv_glEnd(void) {

    LOG("");

    SWGL::Context::getCurrentContext()->getVertexPipeline().end();
}

SWGLAPI void STDCALL glDrv_glEndList(void) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEvalCoord1d(GLdouble u) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEvalCoord1dv(const GLdouble *u) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEvalCoord1f(GLfloat u) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEvalCoord1fv(const GLfloat *u) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEvalCoord2d(GLdouble u, GLdouble v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEvalCoord2dv(const GLdouble *u) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEvalCoord2f(GLfloat u, GLfloat v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEvalCoord2fv(const GLfloat *u) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEvalMesh1(GLenum mode, GLint i1, GLint i2) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEvalMesh2(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEvalPoint1(GLint i) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEvalPoint2(GLint i, GLint j) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glFeedbackBuffer(GLsizei size, GLenum type, GLfloat *buffer) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glFinish(void) {

    LOG("");

    SWGL::Context::getCurrentContext()->getRenderer().finish();
}

SWGLAPI void STDCALL glDrv_glFlush(void) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glFogf(GLenum pname, GLfloat param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glFogfv(GLenum pname, const GLfloat *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glFogi(GLenum pname, GLint param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glFogiv(GLenum pname, const GLint *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glFrontFace(GLenum mode) {

    LOG("Mode: %04x", mode);

    switch (mode) {

    case GL_CW:
    case GL_CCW:
        SWGL::Context::getCurrentContext()->getCulling().setFrontFaceWinding(mode);
        break;

    default:
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar) {

    LOG("Left: %f, Right: %f, Bottom: %f, Top: %f, Near: %f, Far: %f", left, right, bottom, top, zNear, zFar);

    if (zNear < 0.0 || zFar < 0.0 || left == right || bottom == top || zNear == zFar) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    SWGL::Matrix &M = SWGL::Context::getCurrentContext()->getMatrixStack().getCurrentMatrix();

    M *= SWGL::Matrix::getFrustum(

        static_cast<float>(left),
        static_cast<float>(right),
        static_cast<float>(bottom),
        static_cast<float>(top),
        static_cast<float>(zNear),
        static_cast<float>(zFar)
    );
}

SWGLAPI GLuint STDCALL glDrv_glGenLists(GLsizei range) {

    LOG("Unimplemented");
    return 0U;
}

SWGLAPI void STDCALL glDrv_glGetBooleanv(GLenum pname, GLboolean *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetClipPlane(GLenum plane, GLdouble *equation) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetDoublev(GLenum pname, GLdouble *params) {

    LOG("Unimplemented");
}

SWGLAPI GLenum STDCALL glDrv_glGetError(void) {

    GLenum error = SWGL::Context::getCurrentContext()->getError().getState();

    LOG("Returning error: %04x", error);

    return error;
}

SWGLAPI void STDCALL glDrv_glGetFloatv(GLenum pname, GLfloat *params) {

    LOG("Parameter name: %04x, Address: %p", pname, params);

    SWGL::Matrix M;
    switch (pname) {

    case GL_PROJECTION_MATRIX:
        M = SWGL::Context::getCurrentContext()->getMatrixStack().getProjectionMatrix().getTranspose();
        memcpy(

            reinterpret_cast<void *>(params),
            reinterpret_cast<const void *>(M.data()),
            16 * sizeof(float)
        );
        break;

    case GL_MODELVIEW_MATRIX:
        M = SWGL::Context::getCurrentContext()->getMatrixStack().getModelViewMatrix().getTranspose();
        memcpy(

            reinterpret_cast<void *>(params),
            reinterpret_cast<const void *>(M.data()),
            16 * sizeof(float)
        );
        break;

    case GL_TEXTURE_MATRIX:
        M = SWGL::Context::getCurrentContext()->getMatrixStack().getTextureMatrix().getTranspose();
        memcpy(

            reinterpret_cast<void *>(params),
            reinterpret_cast<const void *>(M.data()),
            16 * sizeof(float)
        );
        break;

    case GL_POLYGON_OFFSET_FACTOR:
        params[0] = SWGL::Context::getCurrentContext()->getPolygonOffset().getFactor();
        break;

    case GL_POLYGON_OFFSET_UNITS:
        params[0] = SWGL::Context::getCurrentContext()->getPolygonOffset().getUnits();
        break;

    default:
        LOG("Unimplemented");
        return;
    }
}

SWGLAPI void STDCALL glDrv_glGetIntegerv(GLenum pname, GLint *params) {

    LOG("Param: %04x, Address: %p", pname, params);

    switch (pname) {

    case GL_MAX_TEXTURE_SIZE:
        params[0] = SWGL_MAX_TEXTURE_SIZE;
        break;

    case GL_MAX_TEXTURE_UNITS:
        params[0] = SWGL_MAX_TEXTURE_UNITS;
        break;

    case GL_DOUBLEBUFFER:
        params[0] = GL_TRUE;
        break;

    case GL_MATRIX_MODE:
        params[0] = SWGL::Context::getCurrentContext()->getMatrixStack().getMatrixMode();
        break;

    case GL_VIEWPORT:
        params[0] = SWGL::Context::getCurrentContext()->getViewport().getX();
        params[1] = SWGL::Context::getCurrentContext()->getViewport().getY();
        params[2] = SWGL::Context::getCurrentContext()->getViewport().getWidth();
        params[3] = SWGL::Context::getCurrentContext()->getViewport().getHeight();
        break;

    default:
        LOG("Unimplemented");
        break;
    }
}

SWGLAPI void STDCALL glDrv_glGetLightfv(GLenum light, GLenum pname, GLfloat *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetLightiv(GLenum light, GLenum pname, GLint *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetMapdv(GLenum target, GLenum query, GLdouble *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetMapfv(GLenum target, GLenum query, GLfloat *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetMapiv(GLenum target, GLenum query, GLint *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetMaterialfv(GLenum face, GLenum pname, GLfloat *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetMaterialiv(GLenum face, GLenum pname, GLint *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetPixelMapfv(GLenum map, GLfloat *values) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetPixelMapuiv(GLenum map, GLuint *values) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetPixelMapusv(GLenum map, GLushort *values) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetPolygonStipple(GLubyte *mask) {

    LOG("Unimplemented");
}

SWGLAPI const GLubyte * STDCALL glDrv_glGetString(GLenum name) {

    LOG("String name: %04x", name);

    switch (name) {

    case GL_VENDOR: return reinterpret_cast<const GLubyte *>(SWGL::Context::getCurrentContext()->getLibVendor()); break;
    case GL_RENDERER: return reinterpret_cast<const GLubyte *>(SWGL::Context::getCurrentContext()->getLibRenderer()); break;
    case GL_VERSION: return reinterpret_cast<const GLubyte *>(SWGL::Context::getCurrentContext()->getLibVersion()); break;
    case GL_EXTENSIONS: return reinterpret_cast<const GLubyte *>(SWGL::Context::getCurrentContext()->getLibExtensions()); break;

    default:
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        return reinterpret_cast<const GLubyte *>(0);
    }
}

SWGLAPI void STDCALL glDrv_glGetTexEnvfv(GLenum target, GLenum pname, GLfloat *params) {

    LOG("Target: %04x, Parameter: %04x, Address: %f", target, pname, params);
}

SWGLAPI void STDCALL glDrv_glGetTexEnviv(GLenum target, GLenum pname, GLint *params) {

    LOG("Target: %04x, Parameter: %04x, Address: %f", target, pname, params);
}

SWGLAPI void STDCALL glDrv_glGetTexGendv(GLenum coord, GLenum pname, GLdouble *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetTexGenfv(GLenum coord, GLenum pname, GLfloat *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetTexGeniv(GLenum coord, GLenum pname, GLint *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetTexParameterfv(GLenum target, GLenum pname, GLfloat *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetTexParameteriv(GLenum target, GLenum pname, GLint *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glHint(GLenum target, GLenum mode) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glIndexMask(GLuint mask) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glIndexd(GLdouble c) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glIndexdv(const GLdouble *c) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glIndexf(GLfloat c) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glIndexfv(const GLfloat *c) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glIndexi(GLint c) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glIndexiv(const GLint *c) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glIndexs(GLshort c) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glIndexsv(const GLshort *c) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glInitNames(void) {

    LOG("Unimplemented");
}

SWGLAPI GLboolean STDCALL glDrv_glIsEnabled(GLenum cap) {

    LOG("Capability: %04x", cap);

    bool result = false;
    switch (cap) {

    case GL_SCISSOR_TEST: result = SWGL::Context::getCurrentContext()->getScissor().isEnabled(); break;
    case GL_BLEND: result = SWGL::Context::getCurrentContext()->getBlending().isEnabled(); break;
    case GL_ALPHA_TEST: result = SWGL::Context::getCurrentContext()->getAlphaTesting().isEnabled(); break;
    case GL_CULL_FACE: result = SWGL::Context::getCurrentContext()->getCulling().isEnabled(); break;
    case GL_POLYGON_OFFSET_FILL: result = SWGL::Context::getCurrentContext()->getPolygonOffset().isFillEnabled(); break;
    case GL_DEPTH_TEST: result = SWGL::Context::getCurrentContext()->getDepthTesting().isTestEnabled(); break;

    default:
        LOG("Unimplemented");
        break;
    }

    return result ? GL_TRUE : GL_FALSE;
}

SWGLAPI GLboolean STDCALL glDrv_glIsList(GLuint list) {

    LOG("Unimplemented");
    return GL_FALSE;
}

SWGLAPI void STDCALL glDrv_glLightModelf(GLenum pname, GLfloat param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glLightModelfv(GLenum pname, const GLfloat *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glLightModeli(GLenum pname, GLint param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glLightModeliv(GLenum pname, const GLint *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glLightf(GLenum light, GLenum pname, GLfloat param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glLightfv(GLenum light, GLenum pname, const GLfloat *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glLighti(GLenum light, GLenum pname, GLint param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glLightiv(GLenum light, GLenum pname, const GLint *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glLineStipple(GLint factor, GLushort pattern) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glLineWidth(GLfloat width) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glListBase(GLuint base) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glLoadIdentity(void) {

    LOG("");

    SWGL::Matrix &M = SWGL::Context::getCurrentContext()->getMatrixStack().getCurrentMatrix();

    M = SWGL::Matrix::getIdentity();
}

SWGLAPI void STDCALL glDrv_glLoadMatrixd(const GLdouble *m) {

    LOG("Address: %p", m);

    SWGL::Matrix &M = SWGL::Context::getCurrentContext()->getMatrixStack().getCurrentMatrix();

    for (int i = 0; i < 4; i++, m += 4) {

        M(0, i) = static_cast<float>(m[0]);
        M(1, i) = static_cast<float>(m[1]);
        M(2, i) = static_cast<float>(m[2]);
        M(3, i) = static_cast<float>(m[3]);
    }
}

SWGLAPI void STDCALL glDrv_glLoadMatrixf(const GLfloat *m) {

    LOG("Address: %p", m);

    SWGL::Matrix &M = SWGL::Context::getCurrentContext()->getMatrixStack().getCurrentMatrix();

    for (int i = 0; i < 4; i++, m += 4) {

        M(0, i) = m[0];
        M(1, i) = m[1];
        M(2, i) = m[2];
        M(3, i) = m[3];
    }
}

SWGLAPI void STDCALL glDrv_glLoadName(GLuint name) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glLogicOp(GLenum opcode) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMap1d(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMap1f(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMap2d(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMapGrid1d(GLint un, GLdouble u1, GLdouble u2) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMapGrid1f(GLint un, GLfloat u1, GLfloat u2) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMapGrid2d(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMapGrid2f(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMaterialf(GLenum face, GLenum pname, GLfloat param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMaterialfv(GLenum face, GLenum pname, const GLfloat *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMateriali(GLenum face, GLenum pname, GLint param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMaterialiv(GLenum face, GLenum pname, const GLint *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMatrixMode(GLenum mode) {

    LOG("Mode: %04x", mode);

    SWGL::Context::getCurrentContext()->getMatrixStack().setMatrixMode(mode);
}

SWGLAPI void STDCALL glDrv_glMultMatrixd(const GLdouble *m) {

    LOG("Matrix data address: %p", m);

    if (m != nullptr) {

        SWGL::Matrix &M = SWGL::Context::getCurrentContext()->getMatrixStack().getCurrentMatrix();

        M *= SWGL::Matrix(

            static_cast<float>(m[0]), static_cast<float>(m[4]), static_cast<float>(m[8]), static_cast<float>(m[12]),
            static_cast<float>(m[1]), static_cast<float>(m[5]), static_cast<float>(m[9]), static_cast<float>(m[13]),
            static_cast<float>(m[2]), static_cast<float>(m[6]), static_cast<float>(m[10]), static_cast<float>(m[14]),
            static_cast<float>(m[3]), static_cast<float>(m[7]), static_cast<float>(m[11]), static_cast<float>(m[15])
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultMatrixf(const GLfloat *m) {

    LOG("Matrix data address: %p", m);

    if (m != nullptr) {

        SWGL::Matrix &M = SWGL::Context::getCurrentContext()->getMatrixStack().getCurrentMatrix();

        M *= SWGL::Matrix(

            m[0], m[4], m[8], m[12],
            m[1], m[5], m[9], m[13],
            m[2], m[6], m[10], m[14],
            m[3], m[7], m[11], m[15]
        );
    }
}

SWGLAPI void STDCALL glDrv_glNewList(GLuint list, GLenum mode) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glNormal3b(GLbyte nx, GLbyte ny, GLbyte nz) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glNormal3bv(const GLbyte *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glNormal3dv(const GLdouble *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glNormal3fv(const GLfloat *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glNormal3i(GLint nx, GLint ny, GLint nz) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glNormal3iv(const GLint *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glNormal3s(GLshort nx, GLshort ny, GLshort nz) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glNormal3sv(const GLshort *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar) {

    LOG("Left: %f, Right: %f, Bottom: %f, Top: %f, Near: %f, Far: %f", left, right, bottom, top, zNear, zFar);

    SWGL::Matrix &M = SWGL::Context::getCurrentContext()->getMatrixStack().getCurrentMatrix();

    M *= SWGL::Matrix::getOrtho(

        static_cast<float>(left),
        static_cast<float>(right),
        static_cast<float>(bottom),
        static_cast<float>(top),
        static_cast<float>(zNear),
        static_cast<float>(zFar)
    );
}

SWGLAPI void STDCALL glDrv_glPassThrough(GLfloat token) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPixelMapfv(GLenum map, GLsizei mapsize, const GLfloat *values) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPixelMapuiv(GLenum map, GLsizei mapsize, const GLuint *values) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPixelMapusv(GLenum map, GLsizei mapsize, const GLushort *values) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPixelStoref(GLenum pname, GLfloat param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPixelStorei(GLenum pname, GLint param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPixelTransferf(GLenum pname, GLfloat param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPixelTransferi(GLenum pname, GLint param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPixelZoom(GLfloat xfactor, GLfloat yfactor) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPointSize(GLfloat size) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPolygonMode(GLenum face, GLenum mode) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPolygonStipple(const GLubyte *mask) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPopAttrib(void) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPopMatrix(void) {

    LOG("");

    if (!SWGL::Context::getCurrentContext()->getMatrixStack().canPop()) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_STACK_UNDERFLOW);
        return;
    }

    SWGL::Context::getCurrentContext()->getMatrixStack().pop();
}

SWGLAPI void STDCALL glDrv_glPopName(void) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPushAttrib(GLbitfield mask) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPushMatrix(void) {

    LOG("");

    if (!SWGL::Context::getCurrentContext()->getMatrixStack().canPush()) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_STACK_OVERFLOW);
        return;
    }

    SWGL::Context::getCurrentContext()->getMatrixStack().push();
}

SWGLAPI void STDCALL glDrv_glPushName(GLuint name) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos2d(GLdouble x, GLdouble y) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos2dv(const GLdouble *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos2f(GLfloat x, GLfloat y) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos2fv(const GLfloat *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos2i(GLint x, GLint y) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos2iv(const GLint *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos2s(GLshort x, GLshort y) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos2sv(const GLshort *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos3d(GLdouble x, GLdouble y, GLdouble z) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos3dv(const GLdouble *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos3f(GLfloat x, GLfloat y, GLfloat z) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos3fv(const GLfloat *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos3i(GLint x, GLint y, GLint z) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos3iv(const GLint *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos3s(GLshort x, GLshort y, GLshort z) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos3sv(const GLshort *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos4dv(const GLdouble *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos4fv(const GLfloat *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos4i(GLint x, GLint y, GLint z, GLint w) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos4iv(const GLint *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRasterPos4sv(const GLshort *v) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glReadBuffer(GLenum mode) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels) {

    if (pixels == nullptr)
        return;

    if (width < 0 || height < 0) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    // TODO: Check format and type

    //    if (!SWGL::Context::getCurrentContext()->getRenderer().getRenderSurface().readColorPixels(x, y, width, height, format, type, pixels)) {
    //
    //        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_OPERATION);
    //    }
}

SWGLAPI void STDCALL glDrv_glRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRectdv(const GLdouble *v1, const GLdouble *v2) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRectfv(const GLfloat *v1, const GLfloat *v2) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRecti(GLint x1, GLint y1, GLint x2, GLint y2) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRectiv(const GLint *v1, const GLint *v2) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glRectsv(const GLshort *v1, const GLshort *v2) {

    LOG("Unimplemented");
}

SWGLAPI GLint STDCALL glDrv_glRenderMode(GLenum mode) {

    LOG("Unimplemented");
    return 0;
}

SWGLAPI void STDCALL glDrv_glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z) {

    LOG("Angle: %f, Axis: (%f, %f, %f)", angle, x, y, z);

    SWGL::Matrix &M = SWGL::Context::getCurrentContext()->getMatrixStack().getCurrentMatrix();

    M *= SWGL::Matrix::getRotation(

        static_cast<float>(angle),
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(z)
    );
}

SWGLAPI void STDCALL glDrv_glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {

    LOG("Angle: %f, Axis: (%f, %f, %f)", angle, x, y, z);

    SWGL::Matrix &M = SWGL::Context::getCurrentContext()->getMatrixStack().getCurrentMatrix();

    M *= SWGL::Matrix::getRotation(angle, x, y, z);
}

SWGLAPI void STDCALL glDrv_glScaled(GLdouble x, GLdouble y, GLdouble z) {

    LOG("Axis: (%f, %f, %f)", x, y, z);

    SWGL::Matrix &M = SWGL::Context::getCurrentContext()->getMatrixStack().getCurrentMatrix();

    M *= SWGL::Matrix::getScale(

        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(z)
    );
}

SWGLAPI void STDCALL glDrv_glScalef(GLfloat x, GLfloat y, GLfloat z) {

    LOG("Axis: (%f, %f, %f)", x, y, z);

    SWGL::Matrix &M = SWGL::Context::getCurrentContext()->getMatrixStack().getCurrentMatrix();

    M *= SWGL::Matrix::getScale(x, y, z);
}

SWGLAPI void STDCALL glDrv_glScissor(GLint x, GLint y, GLsizei width, GLsizei height) {

    LOG("X: %d, Y: %d, Width: %d, Height: %d", x, y, width, height);

    if (width < 0 || height < 0) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    SWGL::Context::getCurrentContext()->getScissor().setDimensions(x, y, width, height);
}

SWGLAPI void STDCALL glDrv_glSelectBuffer(GLsizei size, GLuint *buffer) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glShadeModel(GLenum mode) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glStencilFunc(GLenum func, GLint ref, GLuint mask) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glStencilMask(GLuint mask) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glStencilOp(GLenum fail, GLenum zfail, GLenum zpass) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glTexCoord1d(GLdouble s) {

    LOG("S: %f", s);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(s),
            0.0f,
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord1dv(const GLdouble *v) {

    LOG("S: %f", v[0]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(v[0]),
            0.0f,
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord1f(GLfloat s) {

    LOG("S: %f", s);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(s, 0.0f, 0.0f, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord1fv(const GLfloat *v) {

    LOG("S: %f", v[0]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(v[0], 0.0f, 0.0f, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord1i(GLint s) {

    LOG("S: %d", s);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(s),
            0.0f,
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord1iv(const GLint *v) {

    LOG("S: %d", v[0]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(v[0]),
            0.0f,
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord1s(GLshort s) {

    LOG("S: %d", s);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(s),
            0.0f,
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord1sv(const GLshort *v) {

    LOG("S: %d", v[0]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(v[0]),
            0.0f,
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord2d(GLdouble s, GLdouble t) {

    LOG("S: %f, T: %f", s, t);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord2dv(const GLdouble *v) {

    LOG("S: %f, T: %f", v[0], v[1]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord2f(GLfloat s, GLfloat t) {

    LOG("S: %f, T: %f", s, t);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(s, t, 0.0f, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord2fv(const GLfloat *v) {

    LOG("S: %f, T: %f", v[0], v[1]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(v[0], v[1], 0.0f, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord2i(GLint s, GLint t) {

    LOG("S: %d, T: %d", s, t);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord2iv(const GLint *v) {

    LOG("S: %d, T: %d", v[0], v[1]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord2s(GLshort s, GLshort t) {

    LOG("S: %d, T: %d", s, t);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord2sv(const GLshort *v) {

    LOG("S: %d, T: %d", v[0], v[1]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord3d(GLdouble s, GLdouble t, GLdouble r) {

    LOG("S: %f, T: %f, R: %f", s, t, r);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            static_cast<float>(r),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord3dv(const GLdouble *v) {

    LOG("S: %f, T: %f, R: %f", v[0], v[1], v[2]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord3f(GLfloat s, GLfloat t, GLfloat r) {

    LOG("S: %f, T: %f, R: %f", s, t, r);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(s, t, r, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord3fv(const GLfloat *v) {

    LOG("S: %f, T: %f, R: %f", v[0], v[1], v[2]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(v[0], v[1], v[2], 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord3i(GLint s, GLint t, GLint r) {

    LOG("S: %d, T: %d, R: %d", s, t, r);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            static_cast<float>(r),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord3iv(const GLint *v) {

    LOG("S: %d, T: %d, R: %d", v[0], v[1], v[2]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord3s(GLshort s, GLshort t, GLshort r) {

    LOG("S: %d, T: %d, R: %d", s, t, r);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            static_cast<float>(r),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord3sv(const GLshort *v) {

    LOG("S: %d, T: %d, R: %d", v[0], v[1], v[2]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord4d(GLdouble s, GLdouble t, GLdouble r, GLdouble q) {

    LOG("S: %f, T: %f, R: %f, Q: %f", s, t, r, q);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            static_cast<float>(r),
            static_cast<float>(q)
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord4dv(const GLdouble *v) {

    LOG("S: %f, T: %f, R: %f, Q: %f", v[0], v[1], v[2], v[3]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            static_cast<float>(v[3])
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q) {

    LOG("S: %f, T: %f, R: %f, Q: %f", s, t, r, q);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(s, t, r, q)
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord4fv(const GLfloat *v) {

    LOG("S: %f, T: %f, R: %f, Q: %f", v[0], v[1], v[2], v[3]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(v[0], v[1], v[2], v[3])
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord4i(GLint s, GLint t, GLint r, GLint q) {

    LOG("S: %d, T: %d, R: %d, Q: %d", s, t, r, q);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            static_cast<float>(r),
            static_cast<float>(q)
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord4iv(const GLint *v) {

    LOG("S: %d, T: %d, R: %d, Q: %d", v[0], v[1], v[2], v[3]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            static_cast<float>(v[3])
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord4s(GLshort s, GLshort t, GLshort r, GLshort q) {

    LOG("S: %d, T: %d, R: %d, Q: %d", s, t, r, q);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            static_cast<float>(r),
            static_cast<float>(q)
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord4sv(const GLshort *v) {

    LOG("S: %d, T: %d, R: %d, Q: %d", v[0], v[1], v[2], v[3]);

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            static_cast<float>(v[3])
        )
    );
}

SWGLAPI void STDCALL glDrv_glTexEnvf(GLenum target, GLenum pname, GLfloat param) {

    LOG("Target: %04x, Parameter: %04x, Data: %f", target, pname, param);

    switch (target) {

    case GL_TEXTURE_ENV:
        switch (pname) {

        case GL_TEXTURE_ENV_MODE:
            switch (static_cast<GLenum>(param)) {

            case GL_REPLACE:
            case GL_MODULATE:
            case GL_DECAL:
            case GL_BLEND:
            case GL_ADD:
                SWGL::Context::getCurrentContext()->getTextureManager().getActiveTextureEnvironment().mode = static_cast<GLenum>(param);
                break;

            default:
                SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
                return;
            }
            break;

        case GL_TEXTURE_ENV_COLOR:
            LOG("Unimplemented env color");
            break;

        case GL_COMBINE_RGB:
        case GL_COMBINE_ALPHA:
            LOG("Unimplemented Combine RGB/Alpha");
            break;

        case GL_SOURCE0_RGB:
        case GL_SOURCE0_ALPHA:
        case GL_SOURCE1_RGB:
        case GL_SOURCE1_ALPHA:
        case GL_SOURCE2_RGB:
        case GL_SOURCE2_ALPHA:
            LOG("Unimplemented source rgb/alpha");
            break;

        case GL_OPERAND0_ALPHA:
        case GL_OPERAND0_RGB:
        case GL_OPERAND1_ALPHA:
        case GL_OPERAND1_RGB:
        case GL_OPERAND2_ALPHA:
        case GL_OPERAND2_RGB:
            LOG("Unimplemented operand rgb/alpha");
            break;

        default:
            SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
            return;
        }
        break;

    default:
        LOG("Unkown target: %04x\n", target);
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        return;
    }
}

SWGLAPI void STDCALL glDrv_glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params) {

    LOG("Target: %04x, Parameter: %04x, Data Address: %p", target, pname, params);
}

SWGLAPI void STDCALL glDrv_glTexEnvi(GLenum target, GLenum pname, GLint param) {

    LOG("Target: %04x, Parameter: %04x, Data: %d", target, pname, param);
}

SWGLAPI void STDCALL glDrv_glTexEnviv(GLenum target, GLenum pname, const GLint *params) {

    LOG("Target: %04x, Parameter: %04x, Data Address: %p", target, pname, params);
}

SWGLAPI void STDCALL glDrv_glTexGend(GLenum coord, GLenum pname, GLdouble param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glTexGendv(GLenum coord, GLenum pname, const GLdouble *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glTexGenf(GLenum coord, GLenum pname, GLfloat param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glTexGenfv(GLenum coord, GLenum pname, const GLfloat *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glTexGeni(GLenum coord, GLenum pname, GLint param) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glTexGeniv(GLenum coord, GLenum pname, const GLint *params) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glTexImage1D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) {

    LOG("Target: %04x, Level: %d, InternalFormat: %04x, Dimensions: %d x %d pixels, Border: %d, Format: %04x, Type: %04x, Address: %p", target, level, internalFormat, width, height, border, format, type, pixels);

    if (level < 0 || level >= SWGL_MAX_TEXTURE_LOD) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    if (border != 0) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    if (width < 0 || width > SWGL_MAX_TEXTURE_SIZE) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    if (height < 0 || height > SWGL_MAX_TEXTURE_SIZE) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    if (!isPowerOfTwo(width) || !isPowerOfTwo(height)) {

        // Unsupported at the moment
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    SWGL::TextureBaseFormat baseFormat;
    if (!SWGL::Context::getCurrentContext()->getTextureManager().getCompatibleFormat(internalFormat, baseFormat)) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    switch (target) {

    case GL_TEXTURE_2D:
        SWGL::Context::getCurrentContext()->getTextureManager().loadTextureTarget2D(level, width, height, border, baseFormat, format, type, pixels);
        break;

    case GL_TEXTURE_1D:
    case GL_TEXTURE_3D:
    case GL_TEXTURE_CUBE_MAP:
    case GL_PROXY_TEXTURE_1D:
    case GL_PROXY_TEXTURE_2D:
    case GL_PROXY_TEXTURE_3D:
    case GL_PROXY_TEXTURE_CUBE_MAP:
        LOG("Unimplemented texture target %04x", target);
        return;

    default:
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        return;
    }
}

SWGLAPI void STDCALL glDrv_glTexParameterf(GLenum target, GLenum pname, GLfloat param) {

    LOG("Target: %04x, Parameter: %04x, Data: %f", target, pname, param);

    auto &texManager = SWGL::Context::getCurrentContext()->getTextureManager();

    // Get target id
    SWGL::TextureTargetID targetID;
    if (!texManager.getTextureTargetID(target, targetID)) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        return;
    }

    // Make sure that a texture is bound
    if (!texManager.isTextureTargetBound(targetID)) {

        // TODO: What should we do here?
        return;
    }

    //
    // Set texture parameter
    //
    SWGL::TextureParameter &texParams = SWGL::Context::getCurrentContext()->getTextureManager().getTextureParameter(targetID);
    switch (pname) {

    case GL_TEXTURE_MIN_FILTER:
        switch (static_cast<GLenum>(param)) {

        case GL_NEAREST:
        case GL_LINEAR:
        case GL_NEAREST_MIPMAP_NEAREST:
        case GL_LINEAR_MIPMAP_NEAREST:
        case GL_NEAREST_MIPMAP_LINEAR:
        case GL_LINEAR_MIPMAP_LINEAR:
            texParams.minifyFilter = static_cast<GLenum>(param);
            texParams.isUsingMipMapping = texParams.minifyFilter != GL_NEAREST && texParams.minifyFilter != GL_LINEAR;
            if (texParams.isUsingMipMapping) {

                texParams.minifySampler = (texParams.minifyFilter == GL_NEAREST_MIPMAP_NEAREST ||
                    texParams.minifyFilter == GL_NEAREST_MIPMAP_LINEAR) ? &SWGL::sampleTexelsNearest : &SWGL::sampleTexelsLinear;
                texParams.isUsingTrilinearFilter = texParams.minifyFilter == GL_NEAREST_MIPMAP_LINEAR ||
                    texParams.minifyFilter == GL_LINEAR_MIPMAP_LINEAR;
            }
            else {

                texParams.isUsingTrilinearFilter = false;
            }
            break;

        default:
            SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
            return;
        }
        break;

    case GL_TEXTURE_MAG_FILTER:
        switch (static_cast<GLenum>(param)) {

        case GL_NEAREST:
        case GL_LINEAR:
            texParams.magnifyFilter = static_cast<GLenum>(param);
            texParams.magnifySampler = texParams.magnifyFilter == GL_NEAREST ? &SWGL::sampleTexelsNearest : &SWGL::sampleTexelsLinear;
            break;

        default:
            SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
            return;
        }
        break;

    case GL_TEXTURE_WRAP_S:
        switch (static_cast<GLenum>(param)) {

        case GL_CLAMP:
        case GL_CLAMP_TO_EDGE:
        case GL_CLAMP_TO_BORDER:
        case GL_REPEAT:
            texParams.wrappingModeS = static_cast<GLenum>(param);
            break;

        default:
            SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
            return;
        }
        break;

    case GL_TEXTURE_WRAP_T:
        switch (static_cast<GLenum>(param)) {

        case GL_CLAMP:
        case GL_CLAMP_TO_EDGE:
        case GL_CLAMP_TO_BORDER:
        case GL_REPEAT:
            texParams.wrappingModeT = static_cast<GLenum>(param);
            break;

        default:
            SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
            return;
        }
        break;

    case GL_TEXTURE_BORDER_COLOR:
    case GL_TEXTURE_PRIORITY:
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_BASE_LEVEL:
    case GL_TEXTURE_MAX_LEVEL:
        LOG("Unimplemented");
        break;

    default:
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        return;
    }
}

SWGLAPI void STDCALL glDrv_glTexParameterfv(GLenum target, GLenum pname, const GLfloat *params) {

    LOG("Target: %04x, Parameter: %04x, Data Address: %p", target, pname, params);
}

SWGLAPI void STDCALL glDrv_glTexParameteri(GLenum target, GLenum pname, GLint param) {

    LOG("Target: %04x, Parameter: %04x, Data: %d", target, pname, param);
}

SWGLAPI void STDCALL glDrv_glTexParameteriv(GLenum target, GLenum pname, const GLint *params) {

    LOG("Target: %04x, Parameter: %04x, Data Address: %p", target, pname, params);
}

SWGLAPI void STDCALL glDrv_glTranslated(GLdouble x, GLdouble y, GLdouble z) {

    LOG("Vector: (%f, %f, %f)", x, y, z);

    SWGL::Matrix &M = SWGL::Context::getCurrentContext()->getMatrixStack().getCurrentMatrix();

    M *= SWGL::Matrix::getTranslation(

        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(z)
    );
}

SWGLAPI void STDCALL glDrv_glTranslatef(GLfloat x, GLfloat y, GLfloat z) {

    LOG("Vector: (%f, %f, %f)", x, y, z);

    SWGL::Matrix &M = SWGL::Context::getCurrentContext()->getMatrixStack().getCurrentMatrix();

    M *= SWGL::Matrix::getTranslation(x, y, z);
}

SWGLAPI void STDCALL glDrv_glVertex2d(GLdouble x, GLdouble y) {

    LOG("X: %f, Y: %f", x, y);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(x),
            static_cast<float>(y),
            0.0f,
            1.0f
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex2dv(const GLdouble *v) {

    LOG("X: %f, Y: %f", v[0], v[1]);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            0.0f,
            1.0f
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex2f(GLfloat x, GLfloat y) {

    LOG("X: %f, Y: %f", x, y);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(x, y, GLfloat(0.0f), GLfloat(1.0f))
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex2fv(const GLfloat *v) {

    LOG("X: %f, Y: %f", v[0], v[1]);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(v[0], v[1], GLfloat(0.0f), GLfloat(1.0f))
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex2i(GLint x, GLint y) {

    LOG("X: %d, Y: %d", x, y);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(x),
            static_cast<float>(y),
            0.0f,
            1.0f
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex2iv(const GLint *v) {

    LOG("X: %d, Y: %d", v[0], v[1]);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(
            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            0.0f,
            1.0f
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex2s(GLshort x, GLshort y) {

    LOG("X: %d, Y: %d", x, y);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(x),
            static_cast<float>(y),
            0.0f,
            1.0f
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex2sv(const GLshort *v) {

    LOG("X: %d, Y: %d", v[0], v[1]);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            0.0f,
            1.0f
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex3d(GLdouble x, GLdouble y, GLdouble z) {

    LOG("X: %f, Y: %f, Z: %f", x, y, z);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(z),
            1.0f
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex3dv(const GLdouble *v) {

    LOG("X: %f, Y: %f, Z: %f", v[0], v[1], v[2]);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            1.0f
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex3f(GLfloat x, GLfloat y, GLfloat z) {

    LOG("X: %f, Y: %f, Z: %f", x, y, z);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(x, y, z, 1.0f)
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex3fv(const GLfloat *v) {

    LOG("X: %f, Y: %f, Z: %f", v[0], v[1], v[2]);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(v[0], v[1], v[2], 1.0f)
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex3i(GLint x, GLint y, GLint z) {

    LOG("X: %d, Y: %d, Z: %d", x, y, z);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(z),
            1.0f
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex3iv(const GLint *v) {

    LOG("X: %d, Y: %d, Z: %d", v[0], v[1], v[2]);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            1.0f
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex3s(GLshort x, GLshort y, GLshort z) {

    LOG("X: %d, Y: %d, Z: %d", x, y, z);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(z),
            1.0f
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex3sv(const GLshort *v) {

    LOG("X: %d, Y: %d, Z: %d", v[0], v[1], v[2]);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            1.0f
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w) {

    LOG("X: %f, Y: %f, Z: %f, W: %f", x, y, z, w);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(z),
            static_cast<float>(w)
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex4dv(const GLdouble *v) {

    LOG("X: %f, Y: %f, Z: %f, W: %f", v[0], v[1], v[2], v[3]);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            static_cast<float>(v[3])
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {

    LOG("X: %f, Y: %f, Z: %f, W: %f", x, y, z, w);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(x, y, z, w)
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex4fv(const GLfloat *v) {

    LOG("X: %f, Y: %f, Z: %f, W: %f", v[0], v[1], v[2], v[3]);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(v[0], v[1], v[2], v[3])
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex4i(GLint x, GLint y, GLint z, GLint w) {

    LOG("X: %d, Y: %d, Z: %d, W: %d", x, y, z, w);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(z),
            static_cast<float>(w)
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex4iv(const GLint *v) {

    LOG("X: %d, Y: %d, Z: %d, W: %d", v[0], v[1], v[2], v[3]);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            static_cast<float>(v[3])
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex4s(GLshort x, GLshort y, GLshort z, GLshort w) {

    LOG("X: %d, Y: %d, Z: %d, W: %d", x, y, z, w);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(z),
            static_cast<float>(w)
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex4sv(const GLshort *v) {

    LOG("X: %d, Y: %d, Z: %d, W: %d", v[0], v[1], v[2], v[3]);

    auto &vp = SWGL::Context::getCurrentContext()->getVertexPipeline();

    vp.setPosition(

        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            static_cast<float>(v[3])
        )
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {

    LOG("X: %d, Y: %d, Width: %d, Height: %d", x, y, width, height);

    SWGL::Context::getCurrentContext()->getViewport().setDimensions(

        x, y, width, height
    );
}

#pragma endregion
#pragma region Open GL 1.1

SWGLAPI GLboolean STDCALL glDrv_glAreTexturesResident(GLsizei n, const GLuint *textures, GLboolean *residences) {

    LOG("Number of textures: %d, Texture name data: %p, Residences data: %p", n, textures, residences);

    for (int i = 0; i < n; i++) {

        if (!SWGL::Context::getCurrentContext()->getTextureManager().containsTexture(textures[i]))
            return GL_FALSE;
    }

    return GL_TRUE;
}

SWGLAPI void STDCALL glDrv_glArrayElement(GLint i) {

    LOG("Element: %d", i);

    if (i < 0) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    SWGL::Context::getCurrentContext()->getVertexPipeline().setArrayElement(i);
}

SWGLAPI void STDCALL glDrv_glBindTexture(GLenum target, GLuint texture) {

    LOG("Target: %04x, Texture: %d", target, texture);

    auto &texManager = SWGL::Context::getCurrentContext()->getTextureManager();

    // Get target id
    SWGL::TextureTargetID targetID;
    if (!texManager.getTextureTargetID(target, targetID)) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        return;
    }

    //
    // Bind the texture
    //
    if (!SWGL::Context::getCurrentContext()->getTextureManager().bindTexture(targetID, texture)) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_OPERATION);
    }
}

SWGLAPI void STDCALL glDrv_glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {

    LOG("Size: %d, Type: %04x, Stride: %d byte, Address: %p", size, type, stride, pointer);

    SWGL::Context::getCurrentContext()->getVertexPipeline().getColorArrayPointer().setSource(

        pointer,
        type,
        stride,
        size
    );
}

SWGLAPI void STDCALL glDrv_glCopyTexImage1D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glCopyTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glDeleteTextures(GLsizei n, const GLuint *textures) {

    LOG("Number of textures: %d, List Address: %p", n, textures);

    if (textures == nullptr) {

        return;
    }

    if (n < 0) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    auto &texManager = SWGL::Context::getCurrentContext()->getTextureManager();
    for (int i = 0; i < n; i++) {

        auto texName = textures[i];
        if (texName != 0U) {

            texManager.deleteTexture(texName);
        }
    }
}

SWGLAPI void STDCALL glDrv_glDisableClientState(GLenum cap) {

    LOG("Capability: %04x", cap);

    switch (cap) {

    case GL_VERTEX_ARRAY:
        SWGL::Context::getCurrentContext()->getVertexPipeline().getPositionArrayPointer().setEnable(false);
        break;

    case GL_COLOR_ARRAY:
        SWGL::Context::getCurrentContext()->getVertexPipeline().getColorArrayPointer().setEnable(false);
        break;

    case GL_TEXTURE_COORD_ARRAY:
        SWGL::Context::getCurrentContext()->getVertexPipeline().getTexCoordArrayPointer().setEnable(false);
        break;

    default:
        LOG("Unimplemented");
        break;
    }
}

SWGLAPI void STDCALL glDrv_glDrawArrays(GLenum mode, GLint first, GLsizei count) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) {

    LOG("Mode: %04x, Count: %d, Type: %04x, Indices Address: %p", mode, count, type, indices);

    // Check mode
    switch (mode) {

    case GL_POINTS:
    case GL_LINES:
    case GL_LINE_STRIP:
    case GL_LINE_LOOP:
    case GL_TRIANGLES:
    case GL_TRIANGLE_STRIP:
    case GL_TRIANGLE_FAN:
    case GL_QUADS:
    case GL_QUAD_STRIP:
    case GL_POLYGON:
        break;

    default:
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        return;
    }

    // Check count
    if (count < 0) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    // Check type
    switch (type) {

    case GL_UNSIGNED_BYTE:
    case GL_UNSIGNED_SHORT:
    case GL_UNSIGNED_INT:
        break;

    default:
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        return;
    }

    if (indices != nullptr) {

        SWGL::Context::getCurrentContext()->getVertexPipeline().drawArrayElements(mode, count, type, indices);
    }
}

SWGLAPI void STDCALL glDrv_glEdgeFlagPointer(GLsizei stride, const GLvoid *pointer) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glEnableClientState(GLenum cap) {

    LOG("Capability: %04x", cap);

    switch (cap) {

    case GL_VERTEX_ARRAY:
        SWGL::Context::getCurrentContext()->getVertexPipeline().getPositionArrayPointer().setEnable(true);
        break;

    case GL_COLOR_ARRAY:
        SWGL::Context::getCurrentContext()->getVertexPipeline().getColorArrayPointer().setEnable(true);
        break;

    case GL_TEXTURE_COORD_ARRAY:
        SWGL::Context::getCurrentContext()->getVertexPipeline().getTexCoordArrayPointer().setEnable(true);
        break;

    default:
        LOG("Unimplemented");
        break;
    }
}

SWGLAPI void STDCALL glDrv_glGenTextures(GLsizei n, GLuint *textures) {

    LOG("Number of textures: %d, Address: %p", n, textures);

    if (textures == nullptr)
        return;

    if (n < 0) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    SWGL::Context::getCurrentContext()->getTextureManager().getFreeTextureNames(n, textures);
}

SWGLAPI void STDCALL glDrv_glGetPointerv(GLenum pname, GLvoid **params) {

    LOG("Unimplemented");
}

SWGLAPI GLboolean STDCALL glDrv_glIsTexture(GLuint texture) {

    LOG("Unimplemented");
    return GL_FALSE;
}

SWGLAPI void STDCALL glDrv_glIndexPointer(GLenum type, GLsizei stride, const GLvoid *pointer) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glIndexub(GLubyte c) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glIndexubv(const GLubyte *c) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glInterleavedArrays(GLenum format, GLsizei stride, const GLvoid *pointer) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPolygonOffset(GLfloat factor, GLfloat units) {

    LOG("Factor: %f, Units: %f", factor, units);

    SWGL::Context::getCurrentContext()->getPolygonOffset().setOffset(factor, units);
}

SWGLAPI void STDCALL glDrv_glPopClientAttrib(void) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPrioritizeTextures(GLsizei n, const GLuint *textures, const GLclampf *priorities) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glPushClientAttrib(GLbitfield mask) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {

    LOG("Size: %d, Type: %04x, Stride: %d byte, Address: %p", size, type, stride, pointer);

    SWGL::Context::getCurrentContext()->getVertexPipeline().getTexCoordArrayPointer().setSource(

        pointer,
        type,
        stride,
        size
    );
}

SWGLAPI void STDCALL glDrv_glTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) {

    LOG("Target: %04x, Level: %d, X-Offset: %d, Y-Offset: %d, Width: %d, Height: %d, Format: %04x, Type: %04x, Pixels: %p", target, level, xoffset, yoffset, width, height, format, type, pixels);

    if (level < 0 || level >= SWGL_MAX_TEXTURE_LOD) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    if (width < 0 || width > SWGL_MAX_TEXTURE_SIZE) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    if (height < 0 || height > SWGL_MAX_TEXTURE_SIZE) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_VALUE);
        return;
    }

    switch (target) {

    case GL_TEXTURE_2D:
        if (!SWGL::Context::getCurrentContext()->getTextureManager().loadSubTextureTarget2D(level, xoffset, yoffset, width, height, format, type, pixels)) {

            // TODO: What should we do here?
        }
        break;

    case GL_TEXTURE_1D:
    case GL_TEXTURE_3D:
    case GL_TEXTURE_CUBE_MAP:
    case GL_PROXY_TEXTURE_1D:
    case GL_PROXY_TEXTURE_2D:
    case GL_PROXY_TEXTURE_3D:
    case GL_PROXY_TEXTURE_CUBE_MAP:
        LOG("Unimplemented texture target %04x", target);
        return;

    default:
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        return;
    }
}

SWGLAPI void STDCALL glDrv_glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {

    LOG("Size: %d, Type: %04x, Stride: %d byte, Address: %p", size, type, stride, pointer);

    SWGL::Context::getCurrentContext()->getVertexPipeline().getPositionArrayPointer().setSource(

        pointer,
        type,
        stride,
        size
    );
}

#pragma endregion
#pragma region OpenGL 1.2

SWGLAPI void STDCALL glDrv_glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glTexImage3D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *data) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) {

    LOG("Unimplemented");
}

#pragma endregion
#pragma region OpenGL 1.3

SWGLAPI void STDCALL glDrv_glActiveTexture(GLenum texture) {

    LOG("Active Texture: %d", texture - GL_TEXTURE0);

    switch (texture) {

    case GL_TEXTURE0:
    case GL_TEXTURE1:
        SWGL::Context::getCurrentContext()->getTextureManager().setActiveTextureUnit(texture - GL_TEXTURE0);
        break;

    default:
        LOG("Invalid texture unit");
        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glSampleCoverage(GLfloat value, GLboolean invert) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glCompressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glCompressedTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glGetCompressedTexImage(GLenum target, GLint level, GLvoid *pixels) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glClientActiveTexture(GLenum texture) {

    LOG("Texture: %d", texture - GL_TEXTURE0);

    auto texIdx = texture - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS) {

        SWGL::Context::getCurrentContext()->getError().setState(GL_INVALID_ENUM);
        return;
    }

    SWGL::Context::getCurrentContext()->getVertexPipeline().setActiveTexture(texIdx);
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1d(GLenum target, GLdouble s) {

    LOG("Target: %04x, S: %f", target, s);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(s),
            0.0f,
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1dv(GLenum target, const GLdouble *v) {

    LOG("Target: %04x, S: %f", target, v[0]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(v[0]),
            0.0f,
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1f(GLenum target, GLfloat s) {

    LOG("Target: %04x, S: %f", target, s);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(s, 0.0f, 0.0f, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1fv(GLenum target, const GLfloat *v) {

    LOG("Target: %04x, S: %f", target, v[0]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(v[0], 0.0f, 0.0f, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1i(GLenum target, GLint s) {

    LOG("Target: %04x, S: %d", target, s);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(s),
            0.0f,
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1iv(GLenum target, const GLint *v) {

    LOG("Target: %04x, S: %d", target, v[0]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(v[0]),
            0.0f,
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1s(GLenum target, GLshort s) {

    LOG("Target: %04x, S: %d", target, s);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(s),
            0.0f,
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1sv(GLenum target, const GLshort *v) {

    LOG("Target: %04x, S: %d", target, v[0]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(v[0]),
            0.0f,
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2d(GLenum target, GLdouble s, GLdouble t) {

    LOG("Target: %04x, S: %f, T: %f", target, s, t);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2dv(GLenum target, const GLdouble *v) {

    LOG("Target: %04x, S: %f, T: %f", target, v[0], v[1]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2f(GLenum target, GLfloat s, GLfloat t) {

    LOG("Target: %04x, S: %f, T: %f", target, s, t);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(s, t, 0.0f, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2fv(GLenum target, const GLfloat *v) {

    LOG("Target: %04x, S: %f, T: %f", target, v[0], v[1]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(v[0], v[1], 0.0f, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2i(GLenum target, GLint s, GLint t) {

    LOG("Target: %04x, S: %d, T: %d", target, s, t);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2iv(GLenum target, const GLint *v) {

    LOG("Target: %04x, S: %d, T: %d", target, v[0], v[1]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2s(GLenum target, GLshort s, GLshort t) {

    LOG("Target: %04x, S: %d, T: %d", target, s, t);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2sv(GLenum target, const GLshort *v) {

    LOG("Target: %04x, S: %d, T: %d", target, v[0], v[1]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            0.0f,
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3d(GLenum target, GLdouble s, GLdouble t, GLdouble r) {

    LOG("Target: %04x, S: %f, T: %f, R: %f", target, s, t, r);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            static_cast<float>(r),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3dv(GLenum target, const GLdouble *v) {

    LOG("Target: %04x, S: %f, T: %f, R: %f", target, v[0], v[1], v[2]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3f(GLenum target, GLfloat s, GLfloat t, GLfloat r) {

    LOG("Target: %04x, S: %f, T: %f, R: %f", target, s, t, r);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(s, t, r, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3fv(GLenum target, const GLfloat *v) {

    LOG("Target: %04x, S: %f, T: %f, R: %f", target, v[0], v[1], v[2]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(v[0], v[1], v[2], 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3i(GLenum target, GLint s, GLint t, GLint r) {

    LOG("Target: %04x, S: %d, T: %d, R: %d", target, s, t, r);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            static_cast<float>(r),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3iv(GLenum target, const GLint *v) {

    LOG("Target: %04x, S: %d, T: %d, R: %d", target, v[0], v[1], v[2]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3s(GLenum target, GLshort s, GLshort t, GLshort r) {

    LOG("Target: %04x, S: %d, T: %d, R: %d", target, s, t, r);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            static_cast<float>(r),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3sv(GLenum target, const GLshort *v) {

    LOG("Target: %04x, S: %d, T: %d, R: %d", target, v[0], v[1], v[2]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            1.0f
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4d(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q) {

    LOG("Target: %04x, S: %f, T: %f, R: %f, Q: %f", target, s, t, r, q);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            static_cast<float>(r),
            static_cast<float>(q)
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4dv(GLenum target, const GLdouble *v) {

    LOG("Target: %04x, S: %f, T: %f, R: %f, Q: %f", target, v[0], v[1], v[2], v[3]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            static_cast<float>(v[3])
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) {

    LOG("Target: %04x, S: %f, T: %f, R: %f, Q: %f", target, s, t, r, q);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(s, t, r, q)
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4fv(GLenum target, const GLfloat *v) {

    LOG("Target: %04x, S: %f, T: %f, R: %f, Q: %f", target, v[0], v[1], v[2], v[3]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(v[0], v[1], v[2], v[3])
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4i(GLenum target, GLint s, GLint t, GLint r, GLint q) {

    LOG("Target: %04x, S: %d, T: %d, R: %d, Q: %d", target, s, t, r, q);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            static_cast<float>(r),
            static_cast<float>(q)
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4iv(GLenum target, const GLint *v) {

    LOG("Target: %04x, S: %d, T: %d, R: %d, Q: %d", target, v[0], v[1], v[2], v[3]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            static_cast<float>(v[3])
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4s(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q) {

    LOG("Target: %04x, S: %d, T: %d, R: %d, Q: %d", target, s, t, r, q);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(s),
            static_cast<float>(t),
            static_cast<float>(r),
            static_cast<float>(q)
        )
    );
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4sv(GLenum target, const GLshort *v) {

    LOG("Target: %04x, S: %d, T: %d, R: %d, Q: %d", target, v[0], v[1], v[2], v[3]);

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS)
        return;

    SWGL::Context::getCurrentContext()->getVertexPipeline().setTexCoord(

        texIdx,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            static_cast<float>(v[3])
        )
    );
}

SWGLAPI void STDCALL glDrv_glLoadTransposeMatrixf(const GLfloat *m) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glLoadTransposeMatrixd(const GLdouble *m) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMultTransposeMatrixf(const GLfloat *m) {

    LOG("Unimplemented");
}

SWGLAPI void STDCALL glDrv_glMultTransposeMatrixd(const GLdouble *m) {

    LOG("Unimplemented");
}

#pragma endregion



#pragma region Extension: glLockArraysEXT

SWGLAPI void STDCALL glDrv_glLockArrays(GLint first, GLsizei count) {

    LOG("Lock Arrays: First = %d, Count = %d", first, count);

    SWGL::Context::getCurrentContext()->getVertexPipeline().lockArrayElements(first, count);
}

SWGLAPI void STDCALL glDrv_glUnlockArrays() {

    LOG("");
    SWGL::Context::getCurrentContext()->getVertexPipeline().unlockArrayElements();
}

#pragma endregion
