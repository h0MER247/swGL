#include <algorithm>
#include "OpenGL.h"
#include "Context.h"
#include "Log.h"
#include "Matrix.h"
#include "Vector.h"
#include "VertexPipeline.h"
#include "MatrixStack.h"
#include "TextureManager.h"

#define GET_CONTEXT_OR_RETURN(...)                          \
    auto &ctx = SWGL::Context::getCurrentContext();         \
    do {                                                    \
                                                            \
        if (ctx == nullptr) {                               \
                                                            \
            LOG("No context to execute this function!");    \
            return __VA_ARGS__;                             \
        }                                                   \
    }                                                       \
    while(0)

#define MUST_BE_CALLED_OUTSIDE_GL_BEGIN(...)                \
    do {                                                    \
                                                            \
        if (ctx->getVertexPipeline().isInsideGLBegin()) {   \
                                                            \
            LOG("Method was called inside glBegin()!");     \
            ctx->getError().setState(GL_INVALID_OPERATION); \
            return __VA_ARGS__;                             \
        }                                                   \
    }                                                       \
    while(0)

#define MUST_BE_CALLED_INSIDE_GL_BEGIN(...)                 \
    do {                                                    \
                                                            \
        if (!ctx->getVertexPipeline().isInsideGLBegin()) {  \
                                                            \
            LOG("Method wasn't called inside glBegin()!");  \
            ctx->getError().setState(GL_INVALID_OPERATION); \
            return __VA_ARGS__;                             \
        }                                                   \
    }                                                       \
    while(0)

#define CAN_BE_CALLED_INSIDE_GL_BEGIN()

#pragma region OpenGL 1.0

SWGLAPI void STDCALL glDrv_glAccum(GLenum op, GLfloat value) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glAlphaFunc(GLenum func, GLclampf ref) {

    LOG("Alpha Func: %04X, Ref: %f", func, ref);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    switch (func) {

    case GL_NEVER:
    case GL_LESS:
    case GL_EQUAL:
    case GL_LEQUAL:
    case GL_GREATER:
    case GL_NOTEQUAL:
    case GL_GEQUAL:
    case GL_ALWAYS:
        ctx->getAlphaTesting().setAlphaFunc(func, ref);
        break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glBegin(GLenum mode) {

    LOG("Mode: %04x", mode);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

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
        ctx->getVertexPipeline().begin(mode);
        break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glBitmap(GLsizei width, GLsizei height, GLfloat xOrig, GLfloat yOrig, GLfloat xMove, GLfloat yMove, const GLubyte *bitmap) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glBlendFunc(GLenum srcFactor, GLenum dstFactor) {

    LOG("Source Factor: %04X, Destination Factor: %04X", srcFactor, dstFactor);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

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
        ctx->getError().setState(GL_INVALID_ENUM);
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
        ctx->getError().setState(GL_INVALID_ENUM);
        return;
    }

    ctx->getBlending().setFactors(srcFactor, dstFactor);
}

SWGLAPI void STDCALL glDrv_glCallList(GLuint list) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glCallLists(GLsizei n, GLenum type, const GLvoid *lists) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glClear(GLbitfield mask) {

    LOG("Mask: %04x", mask);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if ((mask & GL_COLOR_BUFFER_BIT) != 0) {

        ctx->getRenderer().clearColorBuffer();
    }
    if ((mask & GL_DEPTH_BUFFER_BIT) != 0) {

        ctx->getRenderer().clearDepthBuffer();
    }
}

SWGLAPI void STDCALL glDrv_glClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {

    LOG("Alpha = %f, Red = %f, Green = %f, Blue = %f", alpha, red, green, blue);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    ctx->getClearValues().setClearColor(

        red,
        green,
        blue,
        alpha
    );
}

SWGLAPI void STDCALL glDrv_glClearDepth(GLclampd depth) {

    LOG("Depth = %f", depth);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    ctx->getClearValues().setClearDepth(depth);
}

SWGLAPI void STDCALL glDrv_glClearIndex(GLfloat c) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glClearStencil(GLint s) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glClipPlane(GLenum plane, const GLdouble *equation) {

    LOG("Plane: %d, A: %f, B: %f, C: %f, D: %f", plane - GL_CLIP_PLANE0, equation[0], equation[1], equation[2], equation[3]);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (equation != nullptr) {

        auto &mvMatrix = ctx->getVertexPipeline().getMatrixStack().getModelViewMatrix();

        switch (plane) {

        case GL_CLIP_PLANE0:
        case GL_CLIP_PLANE1:
        case GL_CLIP_PLANE2:
        case GL_CLIP_PLANE3:
        case GL_CLIP_PLANE4:
        case GL_CLIP_PLANE5:
            auto planeEq = SWGL::Vector(

                static_cast<float>(equation[0]),
                static_cast<float>(equation[1]),
                static_cast<float>(equation[2]),
                static_cast<float>(equation[3])
            );
            ctx->getVertexPipeline().getClipper().setUserPlaneEquation(

                plane - GL_CLIP_PLANE0,
                planeEq * mvMatrix.getTransposedInverse()
            );
            break;

        default:
            ctx->getError().setState(GL_INVALID_ENUM);
            break;
        }
    }
}

SWGLAPI void STDCALL glDrv_glColor3b(GLbyte red, GLbyte green, GLbyte blue) {

    LOG("Red: %d, Green: %d, Blue: %d", red, green, blue);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

        SWGL::Vector(red, green, blue, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glColor3fv(const GLfloat *v) {

    LOG("Red: %f, Green: %f, Blue: %f", v[0], v[1], v[2]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

        SWGL::Vector(v[0], v[1], v[2], 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glColor3i(GLint red, GLint green, GLint blue) {

    LOG("Red: %d, Green: %d, Blue: %d", red, green, blue);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

        SWGL::Vector(red, green, blue, alpha)
    );
}

SWGLAPI void STDCALL glDrv_glColor4fv(const GLfloat *v) {

    LOG("Red: %f, Green: %f, Blue: %f, Alpha: %f", v[0], v[1], v[2], v[3]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

        SWGL::Vector(v[0], v[1], v[2], v[3])
    );
}

SWGLAPI void STDCALL glDrv_glColor4i(GLint red, GLint green, GLint blue, GLint alpha) {

    LOG("Red: %d, Green: %d, Blue: %d, Alpha: %d", red, green, blue, alpha);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setColor(

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

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glColorMaterial(GLenum face, GLenum mode) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glCullFace(GLenum mode) {

    LOG("Mode: %04x", mode);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    switch (mode) {

    case GL_FRONT:
    case GL_BACK:
        ctx->getCulling().setCullFace(mode);
        break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glDeleteLists(GLuint list, GLsizei range) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();
}

SWGLAPI void STDCALL glDrv_glDepthFunc(GLenum func) {

    LOG("Function: %04x", func);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    switch (func) {

    case GL_NEVER:
    case GL_LESS:
    case GL_EQUAL:
    case GL_LEQUAL:
    case GL_GREATER:
    case GL_NOTEQUAL:
    case GL_GEQUAL:
    case GL_ALWAYS:
        ctx->getDepthTesting().setTestFunction(func);
        break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glDepthMask(GLboolean flag) {

    LOG("Depth Write Enable: %d", flag == GL_TRUE);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    ctx->getDepthTesting().setWriteEnable(flag == GL_TRUE);
}

SWGLAPI void STDCALL glDrv_glDepthRange(GLclampd zNear, GLclampd zFar) {

    LOG("Near: %f, Far: %f", zNear, zFar);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    ctx->getViewport().setDepthRange(zNear, zFar);
}

SWGLAPI void STDCALL glDrv_glDisable(GLenum cap) {

    LOG("Capability: %04x", cap);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    switch (cap) {

    case GL_CULL_FACE:
        ctx->getCulling().setEnable(false);
        break;

    case GL_DEPTH_TEST:
        ctx->getDepthTesting().setTestEnable(false);
        break;

    case GL_BLEND:
        ctx->getBlending().setEnable(false);
        break;

    case GL_ALPHA_TEST:
        ctx->getAlphaTesting().setEnable(false);
        break;

    case GL_TEXTURE_2D:
        ctx->getTextureManager().setTargetEnable(SWGL::TextureTargetID::Target2D, false);
        break;

    case GL_SCISSOR_TEST:
        ctx->getScissor().setEnable(false);
        break;

    case GL_POLYGON_OFFSET_FILL:
        ctx->getPolygonOffset().setFillEnable(false);
        break;

    case GL_CLIP_PLANE0:
    case GL_CLIP_PLANE1:
    case GL_CLIP_PLANE2:
    case GL_CLIP_PLANE3:
    case GL_CLIP_PLANE4:
    case GL_CLIP_PLANE5:
        ctx->getVertexPipeline().getClipper().setUserPlaneEnable(cap - GL_CLIP_PLANE0, false);
        break;

    default:
        LOG("Unimplemented");
        break;
    }
}

SWGLAPI void STDCALL glDrv_glDrawBuffer(GLenum mode) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEdgeFlag(GLboolean flag) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEdgeFlagv(const GLboolean *flag) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEnable(GLenum cap) {

    LOG("Capability: %04x", cap);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    switch (cap) {

    case GL_CULL_FACE:
        ctx->getCulling().setEnable(true);
        break;

    case GL_DEPTH_TEST:
        ctx->getDepthTesting().setTestEnable(true);
        break;

    case GL_BLEND:
        ctx->getBlending().setEnable(true);
        break;

    case GL_ALPHA_TEST:
        ctx->getAlphaTesting().setEnable(true);
        break;

    case GL_TEXTURE_2D:
        ctx->getTextureManager().setTargetEnable(SWGL::TextureTargetID::Target2D, true);
        break;

    case GL_SCISSOR_TEST:
        ctx->getScissor().setEnable(true);
        break;

    case GL_POLYGON_OFFSET_FILL:
        ctx->getPolygonOffset().setFillEnable(true);
        break;

    case GL_CLIP_PLANE0:
    case GL_CLIP_PLANE1:
    case GL_CLIP_PLANE2:
    case GL_CLIP_PLANE3:
    case GL_CLIP_PLANE4:
    case GL_CLIP_PLANE5:
        ctx->getVertexPipeline().getClipper().setUserPlaneEnable(cap - GL_CLIP_PLANE0, true);
        break;

    default:
        LOG("Unimplemented");
        break;
    }
}

SWGLAPI void STDCALL glDrv_glEnd(void) {

    LOG("");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().end();
}

SWGLAPI void STDCALL glDrv_glEndList(void) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEvalCoord1d(GLdouble u) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEvalCoord1dv(const GLdouble *u) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEvalCoord1f(GLfloat u) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEvalCoord1fv(const GLfloat *u) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEvalCoord2d(GLdouble u, GLdouble v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEvalCoord2dv(const GLdouble *u) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEvalCoord2f(GLfloat u, GLfloat v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEvalCoord2fv(const GLfloat *u) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEvalMesh1(GLenum mode, GLint i1, GLint i2) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEvalMesh2(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEvalPoint1(GLint i) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEvalPoint2(GLint i, GLint j) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glFeedbackBuffer(GLsizei size, GLenum type, GLfloat *buffer) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glFinish(void) {

    LOG("");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    ctx->getRenderer().finish();
}

SWGLAPI void STDCALL glDrv_glFlush(void) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glFogf(GLenum pname, GLfloat param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glFogfv(GLenum pname, const GLfloat *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glFogi(GLenum pname, GLint param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glFogiv(GLenum pname, const GLint *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glFrontFace(GLenum mode) {

    LOG("Mode: %04x", mode);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    switch (mode) {

    case GL_CW:
    case GL_CCW:
        ctx->getCulling().setFrontFaceWinding(mode);
        break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar) {

    LOG("Left: %f, Right: %f, Bottom: %f, Top: %f, Near: %f, Far: %f", left, right, bottom, top, zNear, zFar);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (zNear < 0.0 || zFar < 0.0 || left == right || bottom == top || zNear == zFar) {

        ctx->getError().setState(GL_INVALID_VALUE);
        return;
    }

    auto &matStack = ctx->getVertexPipeline().getMatrixStack();

    SWGL::Matrix &M = matStack.getCurrentMatrix();
    M *= SWGL::Matrix::getFrustum(

        static_cast<float>(left),
        static_cast<float>(right),
        static_cast<float>(bottom),
        static_cast<float>(top),
        static_cast<float>(zNear),
        static_cast<float>(zFar)
    );

    matStack.updateCurrentMatrixStack();
}

SWGLAPI GLuint STDCALL glDrv_glGenLists(GLsizei range) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN(0U);
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN(0U);

    // ...
    return 0U;
}

SWGLAPI void STDCALL glDrv_glGetBooleanv(GLenum pname, GLboolean *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetClipPlane(GLenum plane, GLdouble *equation) {

    LOG("Plane: %d, Equation Address: %p", plane - GL_CLIP_PLANE0, equation);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (equation != nullptr) {

        switch (plane) {

        case GL_CLIP_PLANE0:
        case GL_CLIP_PLANE1:
        case GL_CLIP_PLANE2:
        case GL_CLIP_PLANE3:
        case GL_CLIP_PLANE4:
        case GL_CLIP_PLANE5:
            auto planeEq = ctx->getVertexPipeline().getClipper().getUserPlaneEquation(plane - GL_CLIP_PLANE0);
            equation[0] = planeEq.x();
            equation[1] = planeEq.y();
            equation[2] = planeEq.z();
            equation[3] = planeEq.w();
            break;

        default:
            ctx->getError().setState(GL_INVALID_ENUM);
            break;
        }
    }
}

SWGLAPI void STDCALL glDrv_glGetDoublev(GLenum pname, GLdouble *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI GLenum STDCALL glDrv_glGetError(void) {

    GET_CONTEXT_OR_RETURN(GL_INVALID_OPERATION);
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN(GL_INVALID_OPERATION);

    GLenum error = ctx->getError().getState();
    LOG("Returning error: %04x", error);

    return error;
}

SWGLAPI void STDCALL glDrv_glGetFloatv(GLenum pname, GLfloat *params) {

    LOG("Parameter name: %04x, Address: %p", pname, params);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (params != nullptr) {

        SWGL::Matrix M;
        switch (pname) {

        case GL_PROJECTION_MATRIX:
            M = ctx->getVertexPipeline().getMatrixStack().getProjectionMatrix().getTranspose();
            memcpy(

                reinterpret_cast<void *>(params),
                reinterpret_cast<const void *>(M.data()),
                16 * sizeof(float)
            );
            break;

        case GL_MODELVIEW_MATRIX:
            M = ctx->getVertexPipeline().getMatrixStack().getModelViewMatrix().getTranspose();
            memcpy(

                reinterpret_cast<void *>(params),
                reinterpret_cast<const void *>(M.data()),
                16 * sizeof(float)
            );
            break;

        case GL_TEXTURE_MATRIX:
            M = ctx->getVertexPipeline().getMatrixStack().getTextureMatrix().getTranspose();
            memcpy(

                reinterpret_cast<void *>(params),
                reinterpret_cast<const void *>(M.data()),
                16 * sizeof(float)
            );
            break;

        case GL_POLYGON_OFFSET_FACTOR:
            params[0] = ctx->getPolygonOffset().getFactor();
            break;

        case GL_POLYGON_OFFSET_UNITS:
            params[0] = ctx->getPolygonOffset().getUnits();
            break;

        case GL_COLOR_CLEAR_VALUE:
            params[0] = ctx->getClearValues().getClearColorRed();
            params[1] = ctx->getClearValues().getClearColorGreen();
            params[2] = ctx->getClearValues().getClearColorBlue();
            params[3] = ctx->getClearValues().getClearColorAlpha();
            break;

        default:
            LOG("Unimplemented");
            break;
        }
    }
}

SWGLAPI void STDCALL glDrv_glGetIntegerv(GLenum pname, GLint *params) {

    LOG("Param: %04x, Address: %p", pname, params);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (params != nullptr) {

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
            params[0] = ctx->getVertexPipeline().getMatrixStack().getMatrixMode();
            break;

        case GL_VIEWPORT:
            params[0] = ctx->getViewport().getX();
            params[1] = ctx->getViewport().getY();
            params[2] = ctx->getViewport().getWidth();
            params[3] = ctx->getViewport().getHeight();
            break;

        case GL_MAX_CLIP_PLANES:
            params[0] = SWGL_MAX_CLIP_PLANES;
            break;

        default:
            LOG("Unimplemented");
            break;
        }
    }
}

SWGLAPI void STDCALL glDrv_glGetLightfv(GLenum light, GLenum pname, GLfloat *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetLightiv(GLenum light, GLenum pname, GLint *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetMapdv(GLenum target, GLenum query, GLdouble *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetMapfv(GLenum target, GLenum query, GLfloat *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetMapiv(GLenum target, GLenum query, GLint *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetMaterialfv(GLenum face, GLenum pname, GLfloat *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetMaterialiv(GLenum face, GLenum pname, GLint *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetPixelMapfv(GLenum map, GLfloat *values) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetPixelMapuiv(GLenum map, GLuint *values) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetPixelMapusv(GLenum map, GLushort *values) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetPolygonStipple(GLubyte *mask) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI const GLubyte * STDCALL glDrv_glGetString(GLenum name) {

    LOG("String name: %04x", name);

    GET_CONTEXT_OR_RETURN(nullptr);
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN(nullptr);

    switch (name) {

    case GL_VENDOR: return reinterpret_cast<const GLubyte *>(ctx->getLibVendor()); break;
    case GL_RENDERER: return reinterpret_cast<const GLubyte *>(ctx->getLibRenderer()); break;
    case GL_VERSION: return reinterpret_cast<const GLubyte *>(ctx->getLibVersion()); break;
    case GL_EXTENSIONS: return reinterpret_cast<const GLubyte *>(ctx->getLibExtensions()); break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        return nullptr;
    }
}

SWGLAPI void STDCALL glDrv_glGetTexEnvfv(GLenum target, GLenum pname, GLfloat *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetTexEnviv(GLenum target, GLenum pname, GLint *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetTexGendv(GLenum coord, GLenum pname, GLdouble *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetTexGenfv(GLenum coord, GLenum pname, GLfloat *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetTexGeniv(GLenum coord, GLenum pname, GLint *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetTexParameterfv(GLenum target, GLenum pname, GLfloat *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetTexParameteriv(GLenum target, GLenum pname, GLint *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glHint(GLenum target, GLenum mode) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glIndexMask(GLuint mask) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glIndexd(GLdouble c) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glIndexdv(const GLdouble *c) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glIndexf(GLfloat c) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glIndexfv(const GLfloat *c) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glIndexi(GLint c) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glIndexiv(const GLint *c) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glIndexs(GLshort c) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glIndexsv(const GLshort *c) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glInitNames(void) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI GLboolean STDCALL glDrv_glIsEnabled(GLenum cap) {

    LOG("Capability: %04x", cap);

    GET_CONTEXT_OR_RETURN(GL_FALSE);
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN(GL_FALSE);

    bool result = false;
    switch (cap) {

    case GL_SCISSOR_TEST: result = ctx->getScissor().isEnabled(); break;
    case GL_BLEND: result = ctx->getBlending().isEnabled(); break;
    case GL_ALPHA_TEST: result = ctx->getAlphaTesting().isEnabled(); break;
    case GL_CULL_FACE: result = ctx->getCulling().isEnabled(); break;
    case GL_POLYGON_OFFSET_FILL: result = ctx->getPolygonOffset().isFillEnabled(); break;
    case GL_DEPTH_TEST: result = ctx->getDepthTesting().isTestEnabled(); break;

    case GL_CLIP_PLANE0:
    case GL_CLIP_PLANE1:
    case GL_CLIP_PLANE2:
    case GL_CLIP_PLANE3:
    case GL_CLIP_PLANE4:
    case GL_CLIP_PLANE5:
        result = ctx->getVertexPipeline().getClipper().isUserPlaneEnabled(cap - GL_CLIP_PLANE0);
        break;

    default:
        LOG("Unimplemented");
        break;
    }

    return result ? GL_TRUE : GL_FALSE;
}

SWGLAPI GLboolean STDCALL glDrv_glIsList(GLuint list) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN(GL_FALSE);
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN(GL_FALSE);

    // ...
    return GL_FALSE;
}

SWGLAPI void STDCALL glDrv_glLightModelf(GLenum pname, GLfloat param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glLightModelfv(GLenum pname, const GLfloat *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glLightModeli(GLenum pname, GLint param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glLightModeliv(GLenum pname, const GLint *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glLightf(GLenum light, GLenum pname, GLfloat param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glLightfv(GLenum light, GLenum pname, const GLfloat *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glLighti(GLenum light, GLenum pname, GLint param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glLightiv(GLenum light, GLenum pname, const GLint *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glLineStipple(GLint factor, GLushort pattern) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glLineWidth(GLfloat width) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glListBase(GLuint base) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glLoadIdentity(void) {

    LOG("");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    auto &matStack = ctx->getVertexPipeline().getMatrixStack();

    SWGL::Matrix &M = matStack.getCurrentMatrix();
    M = SWGL::Matrix::getIdentity();

    matStack.updateCurrentMatrixStack();
}

SWGLAPI void STDCALL glDrv_glLoadMatrixd(const GLdouble *m) {

    LOG("Address: %p", m);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (m != nullptr) {

        auto &matStack = ctx->getVertexPipeline().getMatrixStack();

        SWGL::Matrix &M = matStack.getCurrentMatrix();
        for (int i = 0; i < 4; i++, m += 4) {

            M(0, i) = static_cast<float>(m[0]);
            M(1, i) = static_cast<float>(m[1]);
            M(2, i) = static_cast<float>(m[2]);
            M(3, i) = static_cast<float>(m[3]);
        }

        matStack.updateCurrentMatrixStack();
    }
}

SWGLAPI void STDCALL glDrv_glLoadMatrixf(const GLfloat *m) {

    LOG("Address: %p", m);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (m != nullptr) {

        auto &matStack = ctx->getVertexPipeline().getMatrixStack();

        SWGL::Matrix &M = matStack.getCurrentMatrix();
        for (int i = 0; i < 4; i++, m += 4) {

            M(0, i) = m[0];
            M(1, i) = m[1];
            M(2, i) = m[2];
            M(3, i) = m[3];
        }

        matStack.updateCurrentMatrixStack();
    }
}

SWGLAPI void STDCALL glDrv_glLoadName(GLuint name) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glLogicOp(GLenum opcode) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMap1d(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMap1f(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMap2d(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMapGrid1d(GLint un, GLdouble u1, GLdouble u2) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMapGrid1f(GLint un, GLfloat u1, GLfloat u2) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMapGrid2d(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMapGrid2f(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMaterialf(GLenum face, GLenum pname, GLfloat param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMaterialfv(GLenum face, GLenum pname, const GLfloat *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMateriali(GLenum face, GLenum pname, GLint param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMaterialiv(GLenum face, GLenum pname, const GLint *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMatrixMode(GLenum mode) {

    LOG("Mode: %04x", mode);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    ctx->getVertexPipeline().getMatrixStack().setMatrixMode(mode);
}

SWGLAPI void STDCALL glDrv_glMultMatrixd(const GLdouble *m) {

    LOG("Matrix data address: %p", m);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (m != nullptr) {

        auto &matStack = ctx->getVertexPipeline().getMatrixStack();

        SWGL::Matrix &M = matStack.getCurrentMatrix();
        M *= SWGL::Matrix(

            static_cast<float>(m[0]), static_cast<float>(m[4]), static_cast<float>(m[8]), static_cast<float>(m[12]),
            static_cast<float>(m[1]), static_cast<float>(m[5]), static_cast<float>(m[9]), static_cast<float>(m[13]),
            static_cast<float>(m[2]), static_cast<float>(m[6]), static_cast<float>(m[10]), static_cast<float>(m[14]),
            static_cast<float>(m[3]), static_cast<float>(m[7]), static_cast<float>(m[11]), static_cast<float>(m[15])
        );

        matStack.updateCurrentMatrixStack();
    }
}

SWGLAPI void STDCALL glDrv_glMultMatrixf(const GLfloat *m) {

    LOG("Matrix data address: %p", m);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (m != nullptr) {

        auto &matStack = ctx->getVertexPipeline().getMatrixStack();

        SWGL::Matrix &M = matStack.getCurrentMatrix();
        M *= SWGL::Matrix(

            m[0], m[4], m[8], m[12],
            m[1], m[5], m[9], m[13],
            m[2], m[6], m[10], m[14],
            m[3], m[7], m[11], m[15]
        );

        matStack.updateCurrentMatrixStack();
    }
}

SWGLAPI void STDCALL glDrv_glNewList(GLuint list, GLenum mode) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glNormal3b(GLbyte nx, GLbyte ny, GLbyte nz) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glNormal3bv(const GLbyte *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glNormal3dv(const GLdouble *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glNormal3fv(const GLfloat *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glNormal3i(GLint nx, GLint ny, GLint nz) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glNormal3iv(const GLint *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glNormal3s(GLshort nx, GLshort ny, GLshort nz) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glNormal3sv(const GLshort *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar) {

    LOG("Left: %f, Right: %f, Bottom: %f, Top: %f, Near: %f, Far: %f", left, right, bottom, top, zNear, zFar);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    auto &matStack = ctx->getVertexPipeline().getMatrixStack();

    SWGL::Matrix &M = matStack.getCurrentMatrix();
    M *= SWGL::Matrix::getOrtho(

        static_cast<float>(left),
        static_cast<float>(right),
        static_cast<float>(bottom),
        static_cast<float>(top),
        static_cast<float>(zNear),
        static_cast<float>(zFar)
    );

    matStack.updateCurrentMatrixStack();
}

SWGLAPI void STDCALL glDrv_glPassThrough(GLfloat token) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPixelMapfv(GLenum map, GLsizei mapsize, const GLfloat *values) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPixelMapuiv(GLenum map, GLsizei mapsize, const GLuint *values) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPixelMapusv(GLenum map, GLsizei mapsize, const GLushort *values) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPixelStoref(GLenum pname, GLfloat param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPixelStorei(GLenum pname, GLint param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPixelTransferf(GLenum pname, GLfloat param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPixelTransferi(GLenum pname, GLint param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPixelZoom(GLfloat xfactor, GLfloat yfactor) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPointSize(GLfloat size) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPolygonMode(GLenum face, GLenum mode) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPolygonStipple(const GLubyte *mask) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPopAttrib(void) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPopMatrix(void) {

    LOG("");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    auto &matStack = ctx->getVertexPipeline().getMatrixStack();

    if (!matStack.canPop()) {

        ctx->getError().setState(GL_STACK_UNDERFLOW);
        return;
    }

    matStack.pop();
    matStack.updateCurrentMatrixStack();
}

SWGLAPI void STDCALL glDrv_glPopName(void) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPushAttrib(GLbitfield mask) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPushMatrix(void) {

    LOG("");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    auto &matStack = ctx->getVertexPipeline().getMatrixStack();

    if (!matStack.canPush()) {

        ctx->getError().setState(GL_STACK_OVERFLOW);
        return;
    }

    matStack.push();
    matStack.updateCurrentMatrixStack();
}

SWGLAPI void STDCALL glDrv_glPushName(GLuint name) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos2d(GLdouble x, GLdouble y) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos2dv(const GLdouble *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos2f(GLfloat x, GLfloat y) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos2fv(const GLfloat *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos2i(GLint x, GLint y) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos2iv(const GLint *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos2s(GLshort x, GLshort y) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos2sv(const GLshort *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos3d(GLdouble x, GLdouble y, GLdouble z) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos3dv(const GLdouble *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos3f(GLfloat x, GLfloat y, GLfloat z) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos3fv(const GLfloat *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos3i(GLint x, GLint y, GLint z) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos3iv(const GLint *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos3s(GLshort x, GLshort y, GLshort z) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos3sv(const GLshort *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos4dv(const GLdouble *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos4fv(const GLfloat *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos4i(GLint x, GLint y, GLint z, GLint w) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos4iv(const GLint *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRasterPos4sv(const GLshort *v) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glReadBuffer(GLenum mode) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRectdv(const GLdouble *v1, const GLdouble *v2) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRectfv(const GLfloat *v1, const GLfloat *v2) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRecti(GLint x1, GLint y1, GLint x2, GLint y2) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRectiv(const GLint *v1, const GLint *v2) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glRectsv(const GLshort *v1, const GLshort *v2) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI GLint STDCALL glDrv_glRenderMode(GLenum mode) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN(0);
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN(0);

    // ...
    return 0;
}

SWGLAPI void STDCALL glDrv_glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z) {

    LOG("Angle: %f, Axis: (%f, %f, %f)", angle, x, y, z);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    auto &matStack = ctx->getVertexPipeline().getMatrixStack();

    SWGL::Matrix &M = matStack.getCurrentMatrix();
    M *= SWGL::Matrix::getRotation(

        static_cast<float>(angle),
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(z)
    );

    matStack.updateCurrentMatrixStack();
}

SWGLAPI void STDCALL glDrv_glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {

    LOG("Angle: %f, Axis: (%f, %f, %f)", angle, x, y, z);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    auto &matStack = ctx->getVertexPipeline().getMatrixStack();

    SWGL::Matrix &M = matStack.getCurrentMatrix();
    M *= SWGL::Matrix::getRotation(

        angle,
        x,
        y,
        z
    );

    matStack.updateCurrentMatrixStack();
}

SWGLAPI void STDCALL glDrv_glScaled(GLdouble x, GLdouble y, GLdouble z) {

    LOG("Axis: (%f, %f, %f)", x, y, z);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    auto &matStack = ctx->getVertexPipeline().getMatrixStack();

    SWGL::Matrix &M = matStack.getCurrentMatrix();
    M *= SWGL::Matrix::getScale(

        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(z)
    );

    matStack.updateCurrentMatrixStack();
}

SWGLAPI void STDCALL glDrv_glScalef(GLfloat x, GLfloat y, GLfloat z) {

    LOG("Axis: (%f, %f, %f)", x, y, z);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    auto &matStack = ctx->getVertexPipeline().getMatrixStack();

    SWGL::Matrix &M = matStack.getCurrentMatrix();
    M *= SWGL::Matrix::getScale(

        x,
        y,
        z
    );

    matStack.updateCurrentMatrixStack();
}

SWGLAPI void STDCALL glDrv_glScissor(GLint x, GLint y, GLsizei width, GLsizei height) {

    LOG("X: %d, Y: %d, Width: %d, Height: %d", x, y, width, height);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (width < 0 || height < 0) {

        ctx->getError().setState(GL_INVALID_VALUE);
        return;
    }

    ctx->getScissor().setDimensions(x, y, width, height);
}

SWGLAPI void STDCALL glDrv_glSelectBuffer(GLsizei size, GLuint *buffer) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glShadeModel(GLenum mode) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glStencilFunc(GLenum func, GLint ref, GLuint mask) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glStencilMask(GLuint mask) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glStencilOp(GLenum fail, GLenum zfail, GLenum zpass) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glTexCoord1d(GLdouble s) {

    LOG("S: %f", s);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(s, 0.0f, 0.0f, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord1fv(const GLfloat *v) {

    LOG("S: %f", v[0]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(v[0], 0.0f, 0.0f, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord1i(GLint s) {

    LOG("S: %d", s);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(s, t, 0.0f, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord2fv(const GLfloat *v) {

    LOG("S: %f, T: %f", v[0], v[1]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(v[0], v[1], 0.0f, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord2i(GLint s, GLint t) {

    LOG("S: %d, T: %d", s, t);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(s, t, r, 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord3fv(const GLfloat *v) {

    LOG("S: %f, T: %f, R: %f", v[0], v[1], v[2]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(v[0], v[1], v[2], 1.0f)
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord3i(GLint s, GLint t, GLint r) {

    LOG("S: %d, T: %d, R: %d", s, t, r);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(s, t, r, q)
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord4fv(const GLfloat *v) {

    LOG("S: %f, T: %f, R: %f, Q: %f", v[0], v[1], v[2], v[3]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(v[0], v[1], v[2], v[3])
    );
}

SWGLAPI void STDCALL glDrv_glTexCoord4i(GLint s, GLint t, GLint r, GLint q) {

    LOG("S: %d, T: %d, R: %d, Q: %d", s, t, r, q);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    ctx->getVertexPipeline().setTexCoord(

        0U,
        SWGL::Vector(

            static_cast<float>(v[0]),
            static_cast<float>(v[1]),
            static_cast<float>(v[2]),
            static_cast<float>(v[3])
        )
    );
}



SWGLAPI void STDCALL glDrv_glTexEnvModeCommon(const SWGL::ContextPtr &ctx, GLenum param) {

    switch (param) {

    case GL_COMBINE:
    case GL_REPLACE:
    case GL_MODULATE:
    case GL_DECAL:
    case GL_BLEND:
    case GL_ADD:
        ctx->getTextureManager().getActiveTextureEnvironment().mode = param;
        break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glTexEnvColorCommon(const SWGL::ContextPtr &ctx, float r, float g, float b, float a) {

    auto &texEnv = ctx->getTextureManager().getActiveTextureEnvironment();

    texEnv.colorConstA = std::clamp(a, 0.0f, 1.0f);
    texEnv.colorConstR = std::clamp(r, 0.0f, 1.0f);
    texEnv.colorConstG = std::clamp(g, 0.0f, 1.0f);
    texEnv.colorConstB = std::clamp(b, 0.0f, 1.0f);
}

SWGLAPI void STDCALL glDrv_glTexEnvRGBScaleCommon(const SWGL::ContextPtr &ctx, float scaleRGB) {

    if (scaleRGB != 1.0f || scaleRGB != 2.0f || scaleRGB != 4.0f) {

        ctx->getError().setState(GL_INVALID_VALUE);
        return;
    }

    auto &texEnv = ctx->getTextureManager().getActiveTextureEnvironment();

    texEnv.colorScaleRGB = scaleRGB;
}

SWGLAPI void STDCALL glDrv_glTexEnvAlphaScaleCommon(const SWGL::ContextPtr &ctx, float scaleA) {

    if (scaleA != 1.0f || scaleA != 2.0f || scaleA != 4.0f) {

        ctx->getError().setState(GL_INVALID_VALUE);
        return;
    }

    auto &texEnv = ctx->getTextureManager().getActiveTextureEnvironment();

    texEnv.colorScaleA = scaleA;
}

SWGLAPI void STDCALL glDrv_glTexEnvCombineRGBCommon(const SWGL::ContextPtr &ctx, GLenum param) {

    switch (param) {

    case GL_REPLACE:
    case GL_MODULATE:
    case GL_ADD:
    case GL_ADD_SIGNED:
    case GL_INTERPOLATE:
    case GL_SUBTRACT:
    case GL_DOT3_RGB:
    case GL_DOT3_RGBA:
        break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        return;
    }

    auto &texEnv = ctx->getTextureManager().getActiveTextureEnvironment();

    texEnv.combineModeRGB = param;
}

SWGLAPI void STDCALL glDrv_glTexEnvCombineAlphaCommon(const SWGL::ContextPtr &ctx, GLenum param) {

    switch (param) {

    case GL_REPLACE:
    case GL_MODULATE:
    case GL_ADD:
    case GL_ADD_SIGNED:
    case GL_INTERPOLATE:
    case GL_SUBTRACT:
        break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        return;
    }

    auto &texEnv = ctx->getTextureManager().getActiveTextureEnvironment();

    texEnv.combineModeAlpha = param;
}

SWGLAPI void STDCALL glDrv_glTexEnvSourceRGBCommon(const SWGL::ContextPtr &ctx, int sourceIdx, GLenum param) {
    
    if (param < GL_TEXTURE0 ||
        param >= GL_TEXTURE0 + SWGL_MAX_TEXTURE_UNITS) {

        switch (param) {

        case GL_TEXTURE:
        case GL_CONSTANT:
        case GL_PRIMARY_COLOR:
        case GL_PREVIOUS:
            break;

        default:
            ctx->getError().setState(GL_INVALID_ENUM);
            return;
        }
    }

    auto &texEnv = ctx->getTextureManager().getActiveTextureEnvironment();

    texEnv.sourceRGB[sourceIdx] = param;
}

SWGLAPI void STDCALL glDrv_glTexEnvSourceAlphaCommon(const SWGL::ContextPtr &ctx, int sourceIdx, GLenum param) {

    if (param < GL_TEXTURE0 ||
        param >= GL_TEXTURE0 + SWGL_MAX_TEXTURE_UNITS) {

        switch (param) {

        case GL_TEXTURE:
        case GL_CONSTANT:
        case GL_PRIMARY_COLOR:
        case GL_PREVIOUS:
            break;

        default:
            ctx->getError().setState(GL_INVALID_ENUM);
            return;
        }
    }

    auto &texEnv = ctx->getTextureManager().getActiveTextureEnvironment();

    texEnv.sourceAlpha[sourceIdx] = param;
}

SWGLAPI void STDCALL glDrv_glTexEnvOperandRGBCommon(const SWGL::ContextPtr &ctx, int operandIdx, GLenum param) {

    switch (param) {

    case GL_SRC_COLOR:
    case GL_ONE_MINUS_SRC_COLOR:
    case GL_SRC_ALPHA:
    case GL_ONE_MINUS_SRC_ALPHA:
        break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        return;
    }

    auto &texEnv = ctx->getTextureManager().getActiveTextureEnvironment();

    texEnv.operandRGB[operandIdx] = param;
}

SWGLAPI void STDCALL glDrv_glTexEnvOperandAlphaCommon(const SWGL::ContextPtr &ctx, int operandIdx, GLenum param) {

    switch (param) {

    case GL_SRC_ALPHA:
    case GL_ONE_MINUS_SRC_ALPHA:
        break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        return;
    }

    auto &texEnv = ctx->getTextureManager().getActiveTextureEnvironment();

    texEnv.operandAlpha[operandIdx] = param;
}

SWGLAPI void STDCALL glDrv_glTexEnvf(GLenum target, GLenum pname, GLfloat param) {

    LOG("Target: %04x, Parameter: %04x, Data: %f", target, pname, param);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (target != GL_TEXTURE_ENV) {

        ctx->getError().setState(GL_INVALID_ENUM);
        return;
    }

    switch (pname) {

    case GL_TEXTURE_ENV_MODE:
        glDrv_glTexEnvModeCommon(ctx, static_cast<GLenum>(param));
        break;

    case GL_TEXTURE_ENV_COLOR:
        glDrv_glTexEnvColorCommon(ctx, param, param, param, param);
        break;

    case GL_RGB_SCALE:
        glDrv_glTexEnvRGBScaleCommon(ctx, param);
        break;

    case GL_ALPHA_SCALE:
        glDrv_glTexEnvAlphaScaleCommon(ctx, param);
        break;

    case GL_COMBINE_RGB:
        glDrv_glTexEnvCombineRGBCommon(ctx, static_cast<GLenum>(param));
        break;

    case GL_COMBINE_ALPHA:
        glDrv_glTexEnvCombineAlphaCommon(ctx, static_cast<GLenum>(param));
        break;

    case GL_SOURCE0_RGB:
    case GL_SOURCE1_RGB:
    case GL_SOURCE2_RGB:
        glDrv_glTexEnvSourceRGBCommon(ctx, static_cast<int>(pname - GL_SOURCE0_RGB), static_cast<GLenum>(param));
        break;

    case GL_SOURCE0_ALPHA:
    case GL_SOURCE1_ALPHA:
    case GL_SOURCE2_ALPHA:
        glDrv_glTexEnvSourceAlphaCommon(ctx, static_cast<int>(pname - GL_SOURCE0_ALPHA), static_cast<GLenum>(param));
        break;

    case GL_OPERAND0_RGB:
    case GL_OPERAND1_RGB:
    case GL_OPERAND2_RGB:
        glDrv_glTexEnvOperandRGBCommon(ctx, static_cast<int>(pname - GL_OPERAND0_RGB), static_cast<GLenum>(param));
        break;

    case GL_OPERAND0_ALPHA:
    case GL_OPERAND1_ALPHA:
    case GL_OPERAND2_ALPHA:
        glDrv_glTexEnvOperandAlphaCommon(ctx, static_cast<int>(pname - GL_OPERAND0_ALPHA), static_cast<GLenum>(param));
        break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params) {

    LOG("Target: %04x, Parameter: %04x, Data Address: %p", target, pname, params);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (target != GL_TEXTURE_ENV) {

        ctx->getError().setState(GL_INVALID_ENUM);
        return;
    }

    if (params != nullptr) {

        switch (pname) {

        case GL_TEXTURE_ENV_MODE:
            glDrv_glTexEnvModeCommon(ctx, static_cast<GLenum>(params[0]));
            break;

        case GL_TEXTURE_ENV_COLOR:
            glDrv_glTexEnvColorCommon(ctx, params[0], params[1], params[2], params[3]);
            break;

        case GL_RGB_SCALE:
            glDrv_glTexEnvRGBScaleCommon(ctx, params[0]);
            break;

        case GL_ALPHA_SCALE:
            glDrv_glTexEnvAlphaScaleCommon(ctx, params[0]);
            break;

        case GL_COMBINE_RGB:
            glDrv_glTexEnvCombineRGBCommon(ctx, static_cast<GLenum>(params[0]));
            break;

        case GL_COMBINE_ALPHA:
            glDrv_glTexEnvCombineAlphaCommon(ctx, static_cast<GLenum>(params[0]));
            break;

        case GL_SOURCE0_ALPHA:
        case GL_SOURCE1_ALPHA:
        case GL_SOURCE2_ALPHA:
            glDrv_glTexEnvSourceAlphaCommon(ctx, static_cast<int>(pname - GL_SOURCE0_ALPHA), static_cast<GLenum>(params[0]));
            break;

        case GL_SOURCE0_RGB:
        case GL_SOURCE1_RGB:
        case GL_SOURCE2_RGB:
            glDrv_glTexEnvSourceRGBCommon(ctx, static_cast<int>(pname - GL_SOURCE0_RGB), static_cast<GLenum>(params[0]));
            break;

        case GL_OPERAND0_ALPHA:
        case GL_OPERAND1_ALPHA:
        case GL_OPERAND2_ALPHA:
            glDrv_glTexEnvOperandAlphaCommon(ctx, static_cast<int>(pname - GL_OPERAND0_ALPHA), static_cast<GLenum>(params[0]));
            break;

        case GL_OPERAND0_RGB:
        case GL_OPERAND1_RGB:
        case GL_OPERAND2_RGB:
            glDrv_glTexEnvOperandRGBCommon(ctx, static_cast<int>(pname - GL_OPERAND0_RGB), static_cast<GLenum>(params[0]));
            break;

        default:
            ctx->getError().setState(GL_INVALID_ENUM);
            break;
        }
    }
}

SWGLAPI void STDCALL glDrv_glTexEnvi(GLenum target, GLenum pname, GLint param) {

    LOG("Target: %04x, Parameter: %04x, Data: %d", target, pname, param);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (target != GL_TEXTURE_ENV) {

        ctx->getError().setState(GL_INVALID_ENUM);
        return;
    }

    switch (pname) {

    case GL_TEXTURE_ENV_MODE:
        glDrv_glTexEnvModeCommon(ctx, static_cast<GLenum>(param));
        break;

    case GL_TEXTURE_ENV_COLOR:
        glDrv_glTexEnvColorCommon(

            ctx,
            SWGL::Vector::normalizeInteger(param),
            SWGL::Vector::normalizeInteger(param),
            SWGL::Vector::normalizeInteger(param),
            SWGL::Vector::normalizeInteger(param)
        );
        break;

    case GL_RGB_SCALE:
        glDrv_glTexEnvRGBScaleCommon(ctx, SWGL::Vector::normalizeInteger(param));
        break;

    case GL_ALPHA_SCALE:
        glDrv_glTexEnvAlphaScaleCommon(ctx, SWGL::Vector::normalizeInteger(param));
        break;

    case GL_COMBINE_RGB:
        glDrv_glTexEnvCombineRGBCommon(ctx, static_cast<GLenum>(param));
        break;

    case GL_COMBINE_ALPHA:
        glDrv_glTexEnvCombineAlphaCommon(ctx, static_cast<GLenum>(param));
        break;

    case GL_SOURCE0_ALPHA:
    case GL_SOURCE1_ALPHA:
    case GL_SOURCE2_ALPHA:
        glDrv_glTexEnvSourceAlphaCommon(ctx, static_cast<int>(pname - GL_SOURCE0_ALPHA), static_cast<GLenum>(param));
        break;

    case GL_SOURCE0_RGB:
    case GL_SOURCE1_RGB:
    case GL_SOURCE2_RGB:
        glDrv_glTexEnvSourceRGBCommon(ctx, static_cast<int>(pname - GL_SOURCE0_RGB), static_cast<GLenum>(param));
        break;

    case GL_OPERAND0_ALPHA:
    case GL_OPERAND1_ALPHA:
    case GL_OPERAND2_ALPHA:
        glDrv_glTexEnvOperandAlphaCommon(ctx, static_cast<int>(pname - GL_OPERAND0_ALPHA), static_cast<GLenum>(param));
        break;

    case GL_OPERAND0_RGB:
    case GL_OPERAND1_RGB:
    case GL_OPERAND2_RGB:
        glDrv_glTexEnvOperandRGBCommon(ctx, static_cast<int>(pname - GL_OPERAND0_RGB), static_cast<GLenum>(param));
        break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glTexEnviv(GLenum target, GLenum pname, const GLint *params) {

    LOG("Target: %04x, Parameter: %04x, Data Address: %p", target, pname, params);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (target != GL_TEXTURE_ENV) {

        ctx->getError().setState(GL_INVALID_ENUM);
        return;
    }

    if (params != nullptr) {

        switch (pname) {

        case GL_TEXTURE_ENV_MODE:
            glDrv_glTexEnvModeCommon(ctx, static_cast<GLenum>(params[0]));
            break;

        case GL_TEXTURE_ENV_COLOR:
            glDrv_glTexEnvColorCommon(

                ctx,
                SWGL::Vector::normalizeInteger(params[0]),
                SWGL::Vector::normalizeInteger(params[1]),
                SWGL::Vector::normalizeInteger(params[2]),
                SWGL::Vector::normalizeInteger(params[3])
            );
            break;

        case GL_RGB_SCALE:
            glDrv_glTexEnvRGBScaleCommon(ctx, SWGL::Vector::normalizeInteger(params[0]));
            break;

        case GL_ALPHA_SCALE:
            glDrv_glTexEnvAlphaScaleCommon(ctx, SWGL::Vector::normalizeInteger(params[0]));
            break;

        case GL_COMBINE_RGB:
            glDrv_glTexEnvCombineRGBCommon(ctx, static_cast<GLenum>(params[0]));
            break;

        case GL_COMBINE_ALPHA:
            glDrv_glTexEnvCombineAlphaCommon(ctx, static_cast<GLenum>(params[0]));
            break;

        case GL_SOURCE0_ALPHA:
        case GL_SOURCE1_ALPHA:
        case GL_SOURCE2_ALPHA:
            glDrv_glTexEnvSourceAlphaCommon(ctx, static_cast<int>(pname - GL_SOURCE0_ALPHA), static_cast<GLenum>(params[0]));
            break;

        case GL_SOURCE0_RGB:
        case GL_SOURCE1_RGB:
        case GL_SOURCE2_RGB:
            glDrv_glTexEnvSourceRGBCommon(ctx, static_cast<int>(pname - GL_SOURCE0_RGB), static_cast<GLenum>(params[0]));
            break;

        case GL_OPERAND0_ALPHA:
        case GL_OPERAND1_ALPHA:
        case GL_OPERAND2_ALPHA:
            glDrv_glTexEnvOperandAlphaCommon(ctx, static_cast<int>(pname - GL_OPERAND0_ALPHA), static_cast<GLenum>(params[0]));
            break;

        case GL_OPERAND0_RGB:
        case GL_OPERAND1_RGB:
        case GL_OPERAND2_RGB:
            glDrv_glTexEnvOperandRGBCommon(ctx, static_cast<int>(pname - GL_OPERAND0_RGB), static_cast<GLenum>(params[0]));
            break;

        default:
            ctx->getError().setState(GL_INVALID_ENUM);
            break;
        }
    }
}

SWGLAPI void STDCALL glDrv_glTexGend(GLenum coord, GLenum pname, GLdouble param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glTexGendv(GLenum coord, GLenum pname, const GLdouble *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glTexGenf(GLenum coord, GLenum pname, GLfloat param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glTexGenfv(GLenum coord, GLenum pname, const GLfloat *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glTexGeni(GLenum coord, GLenum pname, GLint param) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glTexGeniv(GLenum coord, GLenum pname, const GLint *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glTexImage1D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) {

    LOG("Target: %04x, Level: %d, InternalFormat: %04x, Dimensions: %d x %d pixels, Border: %d, Format: %04x, Type: %04x, Address: %p", target, level, internalFormat, width, height, border, format, type, pixels);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (level < 0 || level > SWGL_MAX_TEXTURE_LOD) {

        ctx->getError().setState(GL_INVALID_VALUE);
        return;
    }

    if (border != 0) {

        ctx->getError().setState(GL_INVALID_VALUE);
        return;
    }

    if (width < 0 || width > SWGL_MAX_TEXTURE_SIZE) {

        ctx->getError().setState(GL_INVALID_VALUE);
        return;
    }

    if (height < 0 || height > SWGL_MAX_TEXTURE_SIZE) {

        ctx->getError().setState(GL_INVALID_VALUE);
        return;
    }

    if (!isPowerOfTwo(width) || !isPowerOfTwo(height)) {

        // Unsupported at the moment
        LOG("Unsupported non power of two sized texture");
        return;
    }

    SWGL::TextureBaseFormat baseFormat;
    if (!ctx->getTextureManager().getCompatibleFormat(internalFormat, baseFormat)) {

        ctx->getError().setState(GL_INVALID_VALUE);
        return;
    }

    switch (target) {

    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    case GL_PROXY_TEXTURE_2D:
    case GL_PROXY_TEXTURE_CUBE_MAP:
        if (!ctx->getTextureManager().loadTextureImage2D(target, level, width, height, border, baseFormat, format, type, pixels)) {

            ctx->getError().setState(GL_INVALID_OPERATION);
        }
        break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glTexParameterCommon(GLenum target, GLenum pname, GLenum param) {

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    auto &texManager = ctx->getTextureManager();

    // Get target id
    SWGL::TextureTargetID targetID;
    if (!texManager.getTextureTargetID(target, targetID)) {

        ctx->getError().setState(GL_INVALID_ENUM);
        return;
    }

    // Make sure that a texture is bound
    if (!texManager.isTextureTargetBound(targetID)) {

        ctx->getError().setState(GL_INVALID_ENUM);
        return;
    }

    //
    // Set texture parameter
    //
    SWGL::TextureParameter &texParams = texManager.getTextureParameter(targetID);
    switch (pname) {

    case GL_TEXTURE_MIN_FILTER:
        switch (param) {

        case GL_NEAREST:
        case GL_LINEAR:
        case GL_NEAREST_MIPMAP_NEAREST:
        case GL_LINEAR_MIPMAP_NEAREST:
        case GL_NEAREST_MIPMAP_LINEAR:
        case GL_LINEAR_MIPMAP_LINEAR:
            texParams.minifyFilter = param;
            if (texParams.minifyFilter != GL_NEAREST &&
                texParams.minifyFilter != GL_LINEAR) {

                texParams.isUsingMipMapping = true;

                // Determine which sampling method we should use for texture minification
                if (texParams.minifyFilter == GL_NEAREST_MIPMAP_NEAREST ||
                    texParams.minifyFilter == GL_NEAREST_MIPMAP_LINEAR) {

                    texParams.minifySampler = &SWGL::sampleTexelsNearest;
                }
                else {

                    texParams.minifySampler = &SWGL::sampleTexelsLinear;
                }

                // Check if we are using trilinear filtering
                if (texParams.minifyFilter == GL_NEAREST_MIPMAP_LINEAR ||
                    texParams.minifyFilter == GL_LINEAR_MIPMAP_LINEAR) {

                    texParams.isUsingTrilinearFilter = true;
                }
                else {

                    texParams.isUsingTrilinearFilter = false;
                }
            }
            else {

                texParams.isUsingMipMapping = false;
                texParams.isUsingTrilinearFilter = false;
            }
            break;

        default:
            ctx->getError().setState(GL_INVALID_ENUM);
            break;
        }
        break;

    case GL_TEXTURE_MAG_FILTER:
        switch (param) {

        case GL_NEAREST:
        case GL_LINEAR:
            texParams.magnifyFilter = param;
            if (texParams.magnifyFilter == GL_NEAREST) {

                texParams.magnifySampler = &SWGL::sampleTexelsNearest;
            }
            else {

                texParams.magnifySampler = &SWGL::sampleTexelsLinear;
            }
            break;

        default:
            ctx->getError().setState(GL_INVALID_ENUM);
            break;
        }
        break;

    case GL_TEXTURE_WRAP_S:
        switch (param) {

        case GL_CLAMP:
        case GL_CLAMP_TO_EDGE:
        case GL_CLAMP_TO_BORDER:
        case GL_REPEAT:
            texParams.wrappingModeS = param;
            break;

        default:
            ctx->getError().setState(GL_INVALID_ENUM);
            break;
        }
        break;

    case GL_TEXTURE_WRAP_T:
        switch (param) {

        case GL_CLAMP:
        case GL_CLAMP_TO_EDGE:
        case GL_CLAMP_TO_BORDER:
        case GL_REPEAT:
            texParams.wrappingModeT = param;
            break;

        default:
            ctx->getError().setState(GL_INVALID_ENUM);
            break;
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
        ctx->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glTexParameterf(GLenum target, GLenum pname, GLfloat param) {

    LOG("Target: %04x, Parameter: %04x, Data: %f", target, pname, param);

    glDrv_glTexParameterCommon(target, pname, static_cast<GLenum>(param));
}

SWGLAPI void STDCALL glDrv_glTexParameterfv(GLenum target, GLenum pname, const GLfloat *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glTexParameteri(GLenum target, GLenum pname, GLint param) {

    LOG("Target: %04x, Parameter: %04x, Data: %d", target, pname, param);

    glDrv_glTexParameterCommon(target, pname, static_cast<GLenum>(param));
}

SWGLAPI void STDCALL glDrv_glTexParameteriv(GLenum target, GLenum pname, const GLint *params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glTranslated(GLdouble x, GLdouble y, GLdouble z) {

    LOG("Vector: (%f, %f, %f)", x, y, z);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    auto &matStack = ctx->getVertexPipeline().getMatrixStack();

    SWGL::Matrix &M = matStack.getCurrentMatrix();
    M *= SWGL::Matrix::getTranslation(

        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(z)
    );

    matStack.updateCurrentMatrixStack();
}

SWGLAPI void STDCALL glDrv_glTranslatef(GLfloat x, GLfloat y, GLfloat z) {

    LOG("Vector: (%f, %f, %f)", x, y, z);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    auto &matStack = ctx->getVertexPipeline().getMatrixStack();

    SWGL::Matrix &M = matStack.getCurrentMatrix();
    M *= SWGL::Matrix::getTranslation(

        x,
        y,
        z
    );

    matStack.updateCurrentMatrixStack();
}

SWGLAPI void STDCALL glDrv_glVertex2d(GLdouble x, GLdouble y) {

    LOG("X: %f, Y: %f", x, y);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
    vp.setPosition(

        SWGL::Vector(x, y, GLfloat(0.0f), GLfloat(1.0f))
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex2fv(const GLfloat *v) {

    LOG("X: %f, Y: %f", v[0], v[1]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
    vp.setPosition(

        SWGL::Vector(v[0], v[1], GLfloat(0.0f), GLfloat(1.0f))
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex2i(GLint x, GLint y) {

    LOG("X: %d, Y: %d", x, y);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
    vp.setPosition(

        SWGL::Vector(x, y, z, 1.0f)
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex3fv(const GLfloat *v) {

    LOG("X: %f, Y: %f, Z: %f", v[0], v[1], v[2]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
    vp.setPosition(

        SWGL::Vector(v[0], v[1], v[2], 1.0f)
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex3i(GLint x, GLint y, GLint z) {

    LOG("X: %d, Y: %d, Z: %d", x, y, z);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
    vp.setPosition(

        SWGL::Vector(x, y, z, w)
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex4fv(const GLfloat *v) {

    LOG("X: %f, Y: %f, Z: %f, W: %f", v[0], v[1], v[2], v[3]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
    vp.setPosition(

        SWGL::Vector(v[0], v[1], v[2], v[3])
    );
    vp.addVertex();
}

SWGLAPI void STDCALL glDrv_glVertex4i(GLint x, GLint y, GLint z, GLint w) {

    LOG("X: %d, Y: %d, Z: %d, W: %d", x, y, z, w);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto &vp = ctx->getVertexPipeline();
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

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    ctx->getViewport().setDimensions(

        x, y, width, height
    );
}

#pragma endregion
#pragma region Open GL 1.1

SWGLAPI GLboolean STDCALL glDrv_glAreTexturesResident(GLsizei n, const GLuint *textures, GLboolean *residences) {

    LOG("Number of textures: %d, Texture name data: %p, Residences data: %p", n, textures, residences);

    GET_CONTEXT_OR_RETURN(GL_FALSE);
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN(GL_FALSE);

    for (int i = 0; i < n; i++) {

        if (!ctx->getTextureManager().containsTexture(textures[i])) {

            return GL_FALSE;
        }
    }

    return GL_TRUE;
}

SWGLAPI void STDCALL glDrv_glArrayElement(GLint i) {

    LOG("Element: %d", i);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    if (i < 0) {

        ctx->getError().setState(GL_INVALID_VALUE);
        return;
    }

    ctx->getVertexPipeline().setArrayElement(i);
}

SWGLAPI void STDCALL glDrv_glBindTexture(GLenum target, GLuint texture) {

    LOG("Target: %04x, Texture: %d", target, texture);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    auto &texManager = ctx->getTextureManager();

    // Get target id
    SWGL::TextureTargetID targetID;
    if (!texManager.getTextureTargetID(target, targetID)) {

        ctx->getError().setState(GL_INVALID_ENUM);
        return;
    }

    //
    // Bind the texture
    //
    if (!ctx->getTextureManager().bindTexture(targetID, texture)) {

        ctx->getError().setState(GL_INVALID_OPERATION);
    }
}

SWGLAPI void STDCALL glDrv_glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {

    LOG("Size: %d, Type: %04x, Stride: %d byte, Address: %p", size, type, stride, pointer);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    ctx->getVertexPipeline().getColorArrayPointer().setSource(

        pointer,
        type,
        stride,
        size
    );
}

SWGLAPI void STDCALL glDrv_glCopyTexImage1D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glCopyTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glDeleteTextures(GLsizei n, const GLuint *textures) {

    LOG("Number of textures: %d, List Address: %p", n, textures);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (textures != nullptr) {

        if (n < 0) {

            ctx->getError().setState(GL_INVALID_VALUE);
            return;
        }

        auto &texManager = ctx->getTextureManager();
        for (int i = 0; i < n; i++) {

            auto texName = textures[i];
            if (texName != 0U) {

                texManager.deleteTexture(texName);
            }
        }
    }
}

SWGLAPI void STDCALL glDrv_glDisableClientState(GLenum cap) {

    LOG("Capability: %04x", cap);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    switch (cap) {

    case GL_VERTEX_ARRAY:
        ctx->getVertexPipeline().getPositionArrayPointer().setEnable(false);
        break;

    case GL_COLOR_ARRAY:
        ctx->getVertexPipeline().getColorArrayPointer().setEnable(false);
        break;

    case GL_TEXTURE_COORD_ARRAY:
        ctx->getVertexPipeline().getTexCoordArrayPointer().setEnable(false);
        break;

    default:
        LOG("Unimplemented");
        break;
    }
}

SWGLAPI void STDCALL glDrv_glDrawArrays(GLenum mode, GLint first, GLsizei count) {

    LOG("Mode: %04x, First: %d, Count: %d", mode, first, count);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

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
        ctx->getError().setState(GL_INVALID_ENUM);
        return;
    }

    // Check count and first element
    if (count < 0 || first < 0) {

        ctx->getError().setState(GL_INVALID_VALUE);
        return;
    }

    ctx->getVertexPipeline().drawArrayElements(mode, first, count);
}

SWGLAPI void STDCALL glDrv_glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) {

    LOG("Mode: %04x, Count: %d, Type: %04x, Indices Address: %p", mode, count, type, indices);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (indices != nullptr) {

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
            ctx->getError().setState(GL_INVALID_ENUM);
            return;
        }

        // Check count
        if (count < 0) {

            ctx->getError().setState(GL_INVALID_VALUE);
            return;
        }

        // Check type
        switch (type) {

        case GL_UNSIGNED_BYTE:
        case GL_UNSIGNED_SHORT:
        case GL_UNSIGNED_INT:
            ctx->getVertexPipeline().drawIndexedArrayElements(mode, count, type, indices);
            break;

        default:
            ctx->getError().setState(GL_INVALID_ENUM);
            break;
        }
    }
}

SWGLAPI void STDCALL glDrv_glEdgeFlagPointer(GLsizei stride, const GLvoid *pointer) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glEnableClientState(GLenum cap) {

    LOG("Capability: %04x", cap);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    switch (cap) {

    case GL_VERTEX_ARRAY:
        ctx->getVertexPipeline().getPositionArrayPointer().setEnable(true);
        break;

    case GL_COLOR_ARRAY:
        ctx->getVertexPipeline().getColorArrayPointer().setEnable(true);
        break;

    case GL_TEXTURE_COORD_ARRAY:
        ctx->getVertexPipeline().getTexCoordArrayPointer().setEnable(true);
        break;

    default:
        LOG("Unimplemented");
        break;
    }
}

SWGLAPI void STDCALL glDrv_glGenTextures(GLsizei n, GLuint *textures) {

    LOG("Number of textures: %d, Address: %p", n, textures);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (textures != nullptr) {

        if (n < 0) {

            ctx->getError().setState(GL_INVALID_VALUE);
            return;
        }

        ctx->getTextureManager().getFreeTextureNames(n, textures);
    }
}

SWGLAPI void STDCALL glDrv_glGetPointerv(GLenum pname, GLvoid **params) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI GLboolean STDCALL glDrv_glIsTexture(GLuint texture) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN(GL_FALSE);
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN(GL_FALSE);

    // ...
    return GL_FALSE;
}

SWGLAPI void STDCALL glDrv_glIndexPointer(GLenum type, GLsizei stride, const GLvoid *pointer) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glIndexub(GLubyte c) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glIndexubv(const GLubyte *c) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glInterleavedArrays(GLenum format, GLsizei stride, const GLvoid *pointer) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glPolygonOffset(GLfloat factor, GLfloat units) {

    LOG("Factor: %f, Units: %f", factor, units);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    ctx->getPolygonOffset().setOffset(factor, units);
}

SWGLAPI void STDCALL glDrv_glPopClientAttrib(void) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();
}

SWGLAPI void STDCALL glDrv_glPrioritizeTextures(GLsizei n, const GLuint *textures, const GLclampf *priorities) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();
}

SWGLAPI void STDCALL glDrv_glPushClientAttrib(GLbitfield mask) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();
}

SWGLAPI void STDCALL glDrv_glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {

    LOG("Size: %d, Type: %04x, Stride: %d byte, Address: %p", size, type, stride, pointer);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    ctx->getVertexPipeline().getTexCoordArrayPointer().setSource(

        pointer,
        type,
        stride,
        size
    );
}

SWGLAPI void STDCALL glDrv_glTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) {

    LOG("Target: %04x, Level: %d, X-Offset: %d, Y-Offset: %d, Width: %d, Height: %d, Format: %04x, Type: %04x, Pixels: %p", target, level, xoffset, yoffset, width, height, format, type, pixels);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    if (level < 0 || level > SWGL_MAX_TEXTURE_LOD) {

        ctx->getError().setState(GL_INVALID_VALUE);
        return;
    }

    if (width < 0 || width > SWGL_MAX_TEXTURE_SIZE) {

        ctx->getError().setState(GL_INVALID_VALUE);
        return;
    }

    if (height < 0 || height > SWGL_MAX_TEXTURE_SIZE) {

        ctx->getError().setState(GL_INVALID_VALUE);
        return;
    }

    switch (target) {

    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
        if (!ctx->getTextureManager().loadSubTextureImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels)) {

            ctx->getError().setState(GL_INVALID_OPERATION);
        }
        break;

    default:
        ctx->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {

    LOG("Size: %d, Type: %04x, Stride: %d byte, Address: %p", size, type, stride, pointer);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    ctx->getVertexPipeline().getPositionArrayPointer().setSource(

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

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glTexImage3D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *data) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

#pragma endregion
#pragma region OpenGL 1.3

SWGLAPI void STDCALL glDrv_glActiveTexture(GLenum texture) {

    LOG("Active Texture: %d", texture - GL_TEXTURE0);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    switch (texture) {

    case GL_TEXTURE0:
    case GL_TEXTURE1:
        ctx->getTextureManager().setActiveTextureUnit(texture - GL_TEXTURE0);
        break;

    default:
        LOG("Invalid texture unit");
        ctx->getError().setState(GL_INVALID_ENUM);
        break;
    }
}

SWGLAPI void STDCALL glDrv_glSampleCoverage(GLfloat value, GLboolean invert) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glCompressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glCompressedTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glGetCompressedTexImage(GLenum target, GLint level, GLvoid *pixels) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glClientActiveTexture(GLenum texture) {

    LOG("Texture: %d", texture - GL_TEXTURE0);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    auto texIdx = texture - GL_TEXTURE0;
    if (texIdx < 0 || texIdx >= SWGL_MAX_TEXTURE_UNITS) {

        ctx->getError().setState(GL_INVALID_ENUM);
        return;
    }

    ctx->getVertexPipeline().setActiveTexture(texIdx);
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1d(GLenum target, GLdouble s) {

    LOG("Target: %04x, S: %f", target, s);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(s),
                0.0f,
                0.0f,
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1dv(GLenum target, const GLdouble *v) {

    LOG("Target: %04x, S: %f", target, v[0]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(v[0]),
                0.0f,
                0.0f,
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1f(GLenum target, GLfloat s) {

    LOG("Target: %04x, S: %f", target, s);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(s, 0.0f, 0.0f, 1.0f)
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1fv(GLenum target, const GLfloat *v) {

    LOG("Target: %04x, S: %f", target, v[0]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(v[0], 0.0f, 0.0f, 1.0f)
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1i(GLenum target, GLint s) {

    LOG("Target: %04x, S: %d", target, s);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(s),
                0.0f,
                0.0f,
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1iv(GLenum target, const GLint *v) {

    LOG("Target: %04x, S: %d", target, v[0]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(v[0]),
                0.0f,
                0.0f,
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1s(GLenum target, GLshort s) {

    LOG("Target: %04x, S: %d", target, s);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(s),
                0.0f,
                0.0f,
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord1sv(GLenum target, const GLshort *v) {

    LOG("Target: %04x, S: %d", target, v[0]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(v[0]),
                0.0f,
                0.0f,
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2d(GLenum target, GLdouble s, GLdouble t) {

    LOG("Target: %04x, S: %f, T: %f", target, s, t);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(s),
                static_cast<float>(t),
                0.0f,
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2dv(GLenum target, const GLdouble *v) {

    LOG("Target: %04x, S: %f, T: %f", target, v[0], v[1]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(v[0]),
                static_cast<float>(v[1]),
                0.0f,
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2f(GLenum target, GLfloat s, GLfloat t) {

    LOG("Target: %04x, S: %f, T: %f", target, s, t);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(s, t, 0.0f, 1.0f)
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2fv(GLenum target, const GLfloat *v) {

    LOG("Target: %04x, S: %f, T: %f", target, v[0], v[1]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(v[0], v[1], 0.0f, 1.0f)
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2i(GLenum target, GLint s, GLint t) {

    LOG("Target: %04x, S: %d, T: %d", target, s, t);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(s),
                static_cast<float>(t),
                0.0f,
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2iv(GLenum target, const GLint *v) {

    LOG("Target: %04x, S: %d, T: %d", target, v[0], v[1]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(v[0]),
                static_cast<float>(v[1]),
                0.0f,
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2s(GLenum target, GLshort s, GLshort t) {

    LOG("Target: %04x, S: %d, T: %d", target, s, t);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(s),
                static_cast<float>(t),
                0.0f,
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord2sv(GLenum target, const GLshort *v) {

    LOG("Target: %04x, S: %d, T: %d", target, v[0], v[1]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(v[0]),
                static_cast<float>(v[1]),
                0.0f,
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3d(GLenum target, GLdouble s, GLdouble t, GLdouble r) {

    LOG("Target: %04x, S: %f, T: %f, R: %f", target, s, t, r);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(s),
                static_cast<float>(t),
                static_cast<float>(r),
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3dv(GLenum target, const GLdouble *v) {

    LOG("Target: %04x, S: %f, T: %f, R: %f", target, v[0], v[1], v[2]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(v[0]),
                static_cast<float>(v[1]),
                static_cast<float>(v[2]),
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3f(GLenum target, GLfloat s, GLfloat t, GLfloat r) {

    LOG("Target: %04x, S: %f, T: %f, R: %f", target, s, t, r);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(s, t, r, 1.0f)
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3fv(GLenum target, const GLfloat *v) {

    LOG("Target: %04x, S: %f, T: %f, R: %f", target, v[0], v[1], v[2]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(v[0], v[1], v[2], 1.0f)
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3i(GLenum target, GLint s, GLint t, GLint r) {

    LOG("Target: %04x, S: %d, T: %d, R: %d", target, s, t, r);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(s),
                static_cast<float>(t),
                static_cast<float>(r),
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3iv(GLenum target, const GLint *v) {

    LOG("Target: %04x, S: %d, T: %d, R: %d", target, v[0], v[1], v[2]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(v[0]),
                static_cast<float>(v[1]),
                static_cast<float>(v[2]),
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3s(GLenum target, GLshort s, GLshort t, GLshort r) {

    LOG("Target: %04x, S: %d, T: %d, R: %d", target, s, t, r);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(s),
                static_cast<float>(t),
                static_cast<float>(r),
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord3sv(GLenum target, const GLshort *v) {

    LOG("Target: %04x, S: %d, T: %d, R: %d", target, v[0], v[1], v[2]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(v[0]),
                static_cast<float>(v[1]),
                static_cast<float>(v[2]),
                1.0f
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4d(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q) {

    LOG("Target: %04x, S: %f, T: %f, R: %f, Q: %f", target, s, t, r, q);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(s),
                static_cast<float>(t),
                static_cast<float>(r),
                static_cast<float>(q)
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4dv(GLenum target, const GLdouble *v) {

    LOG("Target: %04x, S: %f, T: %f, R: %f, Q: %f", target, v[0], v[1], v[2], v[3]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(v[0]),
                static_cast<float>(v[1]),
                static_cast<float>(v[2]),
                static_cast<float>(v[3])
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) {

    LOG("Target: %04x, S: %f, T: %f, R: %f, Q: %f", target, s, t, r, q);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(s, t, r, q)
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4fv(GLenum target, const GLfloat *v) {

    LOG("Target: %04x, S: %f, T: %f, R: %f, Q: %f", target, v[0], v[1], v[2], v[3]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(v[0], v[1], v[2], v[3])
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4i(GLenum target, GLint s, GLint t, GLint r, GLint q) {

    LOG("Target: %04x, S: %d, T: %d, R: %d, Q: %d", target, s, t, r, q);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(s),
                static_cast<float>(t),
                static_cast<float>(r),
                static_cast<float>(q)
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4iv(GLenum target, const GLint *v) {

    LOG("Target: %04x, S: %d, T: %d, R: %d, Q: %d", target, v[0], v[1], v[2], v[3]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(v[0]),
                static_cast<float>(v[1]),
                static_cast<float>(v[2]),
                static_cast<float>(v[3])
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4s(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q) {

    LOG("Target: %04x, S: %d, T: %d, R: %d, Q: %d", target, s, t, r, q);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(s),
                static_cast<float>(t),
                static_cast<float>(r),
                static_cast<float>(q)
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glMultiTexCoord4sv(GLenum target, const GLshort *v) {

    LOG("Target: %04x, S: %d, T: %d, R: %d, Q: %d", target, v[0], v[1], v[2], v[3]);

    GET_CONTEXT_OR_RETURN();
    CAN_BE_CALLED_INSIDE_GL_BEGIN();

    auto texIdx = target - GL_TEXTURE0;
    if (texIdx >= 0U && texIdx < SWGL_MAX_TEXTURE_UNITS) {

        ctx->getVertexPipeline().setTexCoord(

            texIdx,
            SWGL::Vector(

                static_cast<float>(v[0]),
                static_cast<float>(v[1]),
                static_cast<float>(v[2]),
                static_cast<float>(v[3])
            )
        );
    }
}

SWGLAPI void STDCALL glDrv_glLoadTransposeMatrixf(const GLfloat *m) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glLoadTransposeMatrixd(const GLdouble *m) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMultTransposeMatrixf(const GLfloat *m) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

SWGLAPI void STDCALL glDrv_glMultTransposeMatrixd(const GLdouble *m) {

    LOG("Unimplemented");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    // ...
}

#pragma endregion



#pragma region Extension: glLockArraysEXT

SWGLAPI void STDCALL glDrv_glLockArrays(GLint first, GLsizei count) {

    LOG("Lock Arrays: First = %d, Count = %d", first, count);

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    ctx->getVertexPipeline().lockArrayElements(first, count);
}

SWGLAPI void STDCALL glDrv_glUnlockArrays() {

    LOG("");

    GET_CONTEXT_OR_RETURN();
    MUST_BE_CALLED_OUTSIDE_GL_BEGIN();

    ctx->getVertexPipeline().unlockArrayElements();
}

#pragma endregion
