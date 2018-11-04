#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include "Vector.h"
#include "Vertex.h"
#include "OpenGL.h"
#include "Log.h"

namespace SWGL {

    //
    // Color Mask
    //
    class ColorMask {

    public:
        ColorMask()

            : m_mask(-1) {

        }
        ~ColorMask() = default;

    public:
        void setMask(bool alpha, bool red, bool green, bool blue) {

            int mask = 0;

            if (alpha) { mask |= 0xff000000; }
            if (red) { mask |= 0x00ff0000; }
            if (green) { mask |= 0x0000ff00; }
            if (blue) { mask |= 0x000000ff; }

            m_mask = mask;
        }

    public:
        int getMask() { return m_mask; }

    private:
        int m_mask;
    };

    //
    // Polygon offset state (Page 77, 3.5.5 Depth Offset, glspec13.pdf)
    //
    class PolygonOffset {

    public:
        PolygonOffset()

            : m_isFillEnabled(false),
              m_factor(0.0f),
              m_units(0.0f),
              m_rTimesUnits(0.0f) {

        }
        ~PolygonOffset() = default;

    public:
        void setFillEnable(bool isEnabled) {

            m_isFillEnabled = isEnabled;
        }

        void setOffset(float factor, float units) {

            static constexpr float r = 1.0f / 16777216.0f;

            m_factor = factor;
            m_units = units;
            m_rTimesUnits = r * units;
        }

    public:
        bool isFillEnabled() {

            return m_isFillEnabled;
        }

        float getFactor() {

            return m_factor;
        }

        float getUnits() {

            return m_units;
        }

        float getRTimesUnits() {

            return m_rTimesUnits;
        }

    private:
        bool m_isFillEnabled;
        float m_factor;
        float m_units;
        float m_rTimesUnits;
    };



    //
    // Blending state (Page 161, 4.1.7 Blending, glspec13.pdf)
    //
    class Blending {

    public:
        Blending()
        
            : m_isEnabled(false),
              m_srcFactor(GL_ONE),
              m_dstFactor(GL_ZERO) {

        }
        ~Blending() = default;

    public:
        void setEnable(bool isEnabled) {

            m_isEnabled = isEnabled;
        }

        void setFactors(GLenum srcFactor, GLenum dstFactor) {

            m_srcFactor = srcFactor;
            m_dstFactor = dstFactor;
        }

    public:
        bool isEnabled() {

            return m_isEnabled;
        }

        GLenum getSourceFactor() {

            return m_srcFactor;
        }

        GLenum getDestinationFactor() {

            return m_dstFactor;
        }

    private:
        bool m_isEnabled;
        GLenum m_srcFactor;
        GLenum m_dstFactor;
    };



    //
    // Alpha testing state (Page 159, 4.1.4 Alpha test, glspec13.pdf)
    //
    class AlphaTesting {

    public:
        AlphaTesting()
        
            : m_isEnabled(false),
              m_testFunc(GL_ALWAYS), 
              m_refValue(0.0f) {

        }
        ~AlphaTesting() = default;

    public:
        void setEnable(bool isEnabled) {

            m_isEnabled = isEnabled;
        }

        void setAlphaFunc(GLenum testFunc, GLclampf refValue) {

            m_testFunc = testFunc;
            m_refValue = std::clamp(refValue, 0.0f, 1.0f);
        }

    public:
        bool isEnabled() {

            return m_isEnabled;
        }

        GLenum getTestFunction() {

            return m_testFunc;
        }

        GLclampf getReferenceValue() {

            return m_refValue;
        }

    private:
        bool m_isEnabled;
        GLenum m_testFunc;
        GLclampf m_refValue;
    };



    //
    // Depth buffer test state (Page 161, 4.1.6 Depth buffer test, glspec13.pdf)
    //
    class DepthTesting {

    public:
        DepthTesting()

            : m_isTestEnabled(false),
              m_isWriteEnabled(true),
              m_testFunc(GL_LESS) {

        }
        ~DepthTesting() = default;

    public:
        void setTestEnable(bool isEnabled) {

            m_isTestEnabled = isEnabled;
        }

        void setWriteEnable(bool isEnabled) {

            m_isWriteEnabled = isEnabled;
        }

        void setTestFunction(GLenum testFunc) {

            m_testFunc = testFunc;
        }

    public:
        bool isTestEnabled() {

            return m_isTestEnabled;
        }

        bool isWriteEnabled() {

            return m_isWriteEnabled;
        }

        GLenum getTestFunction() {

            return m_testFunc;
        }

    private:
        bool m_isTestEnabled;
        bool m_isWriteEnabled;
        GLenum m_testFunc;
    };



    //
    // Scissor test state (Page 158, 4.1.2 Scissor test, glspec13.pdf)
    //
    class Scissor {

    public:
        Scissor()

            : m_isEnabled(false),
              m_minX(0),
              m_minY(0),
              m_maxX(0),
              m_maxY(0) {

        }
        ~Scissor() = default;

    public:
        void setDimensions(int x, int y, int width, int height) {

            m_minX = x;
            m_minY = y;
            m_maxX = x + width;
            m_maxY = y + height;
        }

        void setEnable(bool isEnabled) {

            m_isEnabled = isEnabled;
        }

    public:
        bool isEnabled() {

            return m_isEnabled;
        }

        int getMinX() { return m_minX; }
        int getMaxX() { return m_maxX; }
        int getMinY() { return m_minY; }
        int getMaxY() { return m_maxY; }
        int getWidth() { return m_maxX - m_minX; }
        int getHeight() { return m_maxY - m_minY; }

        void cut(int &minX, int &minY, int &maxX, int &maxY) {

            minX = std::max(minX, m_minX);
            minY = std::max(minY, m_minY);
            maxX = std::min(maxX, m_maxX);
            maxY = std::min(maxY, m_maxY);
        }

    private:
        bool m_isEnabled;
        int m_minX, m_minY;
        int m_maxX, m_maxY;
    };



    //
    //
    //
    class ClearValues {

    public:
        ClearValues()

            : m_color(0),
              m_depth(0x00ffffff) {

        }
        ~ClearValues() = default;

    public:
        void setClearColor(float red, float green, float blue, float alpha) {

            m_colorRed = std::clamp(red, 0.0f, 1.0f);
            m_colorGreen = std::clamp(green, 0.0f, 1.0f);
            m_colorBlue = std::clamp(blue, 0.0f, 1.0f);
            m_colorAlpha = std::clamp(alpha, 0.0f, 1.0f);

            int r = static_cast<int>(255.0f * m_colorRed);
            int g = static_cast<int>(255.0f * m_colorGreen);
            int b = static_cast<int>(255.0f * m_colorBlue);
            int a = static_cast<int>(255.0f * m_colorAlpha);

            m_color = (a << 24) | (r << 16) | (g << 8) | b;
        }

        void setClearDepth(double depth) {

            auto depthValue = std::clamp(static_cast<float>(depth), 0.0f, 1.0f);

            m_depth = static_cast<unsigned int>(depthValue * 16777215.0f);
        }

    public:
        unsigned int getClearColor() {

            return m_color;
        }

        unsigned int getClearDepth() {

            return m_depth;
        }

    public:
        float getClearColorRed() {

            return m_colorRed;
        }

        float getClearColorGreen() {

            return m_colorGreen;
        }

        float getClearColorBlue() {

            return m_colorBlue;
        }

        float getClearColorAlpha() {

            return m_colorAlpha;
        }

    private:
        unsigned int m_color;
        unsigned int m_depth;

    private:
        float m_colorRed;
        float m_colorGreen;
        float m_colorBlue;
        float m_colorAlpha;
    };



    //
    //
    //
    class Culling {

    public:
        Culling()

            : m_isEnabled(false),
              m_frontFaceWinding(GL_CCW),
              m_cullFace(GL_BACK) {

        }
        ~Culling() = default;

    public:
        void setEnable(bool isEnabled) {

            m_isEnabled = isEnabled;
        }

        void setFrontFaceWinding(GLenum frontFaceWinding) {

            m_frontFaceWinding = frontFaceWinding;
        }

        void setCullFace(GLenum cullFace) {

            m_cullFace = cullFace;
        }


    public:
        bool isEnabled() {

            return m_isEnabled;
        }

        GLenum getFrontFaceWinding() {

            return m_frontFaceWinding;
        }

        GLenum getCullFace() {

            return m_cullFace;
        }

    public:
        bool isTriangleVisible(Vertex &v1, Vertex &v2, Vertex &v3) {

            if (m_isEnabled) {
            
                if(m_cullFace != GL_FRONT_AND_BACK) {

                    float a = (v1.posProj.x() * v2.posProj.y() - v1.posProj.y() * v2.posProj.x()) * v3.posProj.w() +
                              (v1.posProj.y() * v3.posProj.x() - v1.posProj.x() * v3.posProj.y()) * v2.posProj.w() +
                              (v2.posProj.x() * v3.posProj.y() - v3.posProj.x() * v2.posProj.y()) * v1.posProj.w();

                    return (a > 0.0f) ^ (m_cullFace == GL_FRONT) ^ (m_frontFaceWinding == GL_CW);
                }

                return false;
            }

            return true;
        }

    private:
        bool m_isEnabled;
        GLenum m_frontFaceWinding;
        GLenum m_cullFace;
    };



    //
    //
    //
    class GLError {

    public:
        GLError()

            : m_state(GL_NO_ERROR) {

        }
        ~GLError() = default;

    public:
        void setState(GLenum error) {

            LOG("==> Set GL Error: %04x", error);

            switch (error) {

            case GL_INVALID_ENUM:
            case GL_INVALID_VALUE:
            case GL_INVALID_OPERATION:
            case GL_STACK_OVERFLOW:
            case GL_STACK_UNDERFLOW:
            case GL_OUT_OF_MEMORY:
                m_state |= 1 << (error & 0x07);
                break;
            }
        }

    public:
        GLenum getState() {

            if (m_state == GL_NO_ERROR) {

                return GL_NO_ERROR;
            }

            int error = m_state & ~(m_state - 1);
            m_state ^= error;

            return GL_INVALID_ENUM + (error - 1);
        }

    private:
        int m_state;
    };

    //
    //
    //
    class Viewport {

    public:
        Viewport()

            : m_x(0),
              m_y(0),
              m_width(0),
              m_height(0),
              m_scaleZ(0.0f), m_translateZ(0.0f),
              m_scaleY(0.0f), m_translateY(0.0f),
              m_scaleX(0.0f), m_translateX(0.0f) {
                
            setDepthRange(0.0f, 1.0f);
        }
        ~Viewport() = default;

    public:
        void setDimensions(int x, int y, int width, int height) {

            // Clamp width and height to a maximum of 2048 as this is the maximum
            // size that can be rasterized without risking integer overflows.
            m_width = std::clamp(width, 0, 2048);
            m_height = std::clamp(height, 0, 2048);
            m_x = x;
            m_y = y;

            m_scaleX = static_cast<float>(m_width) * 0.5f;
            m_scaleY = static_cast<float>(m_height) * 0.5f;

            // The offset of -0.5f is needed in order for the pixel center to be
            // correctly at (0.5,0.5). If this is missing texturing looks wrong.
            // TODO: Is that the right way to do this?
            m_translateX = (static_cast<float>(x) + m_scaleX) - 0.5f;
            m_translateY = (static_cast<float>(y) + m_scaleY) - 0.5f;
        }

        void setDepthRange(double zNear, double zFar) {

            float n = std::clamp(static_cast<float>(zNear), 0.0f, 1.0f);
            float f = std::clamp(static_cast<float>(zFar), 0.0f, 1.0f);

            m_scaleZ = (f - n) * 0.5f;
            m_translateZ = (f + n) * 0.5f;
        }
        
    public:
        int getX() {

            return m_x;
        }

        int getY() {

            return m_y;
        }

        int getWidth() {

            return m_width;
        }

        int getHeight() {

            return m_height;
        }

    public:
        void transform(Vector &vector) {

            vector.z() = (vector.z() * m_scaleZ) + m_translateZ;
            vector.y() = (vector.y() * m_scaleY) + m_translateY;
            vector.x() = (vector.x() * m_scaleX) + m_translateX;
        }

    private:
        int m_x, m_y;
        int m_width, m_height;
        float m_scaleZ, m_translateZ;
        float m_scaleY, m_translateY;
        float m_scaleX, m_translateX;
    };
}
