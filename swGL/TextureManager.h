#pragma once

// TODO: Refactor this "manager" as it "manages" way too much stuff :D
#include <map>
#include <deque>
#include <array>
#include <vector>
#include <memory>
#include "Defines.h"
#include "SIMD.h"
#include "OpenGL.h"
#include "ContextTypes.h"
#include "AlignedAllocator.h"

namespace SWGL {

    // Forward declarations
    struct TextureObject;
    struct TextureData;
    struct TextureMipMap;
    struct TextureParameter;
    struct TextureCoordinates;
    struct ARGBColor;

    // Type aliases
    using TextureObjectPtr = std::shared_ptr<TextureObject>;
    using TextureDataPtr = std::shared_ptr<TextureData>;
    using TexturePixels = std::vector<unsigned int, AlignedAllocator<unsigned int, 16>>;
    using SamplerMethod = void(*)(TextureMipMap &, TextureParameter &, TextureCoordinates &, ARGBColor &);

    // Texture sampling methods from TextureSampler.cpp
    extern void sampleTexelsNearest(TextureMipMap &texMipMap, TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &color);
    extern void sampleTexelsLinear(TextureMipMap &texMipMap, TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &color);

    // This describes the format in which swGL stores a texture internally
    enum class TextureBaseFormat : unsigned int {

        Alpha,
        Luminance,
        LuminanceAlpha,
        Intensity,
        RGB,
        RGBA
    };

    // Stores environment data of a texture unit (set with glTexEnv())
    struct TextureEnvironment {

        // Environment mode
        GLenum mode = GL_MODULATE;

        // Combine Mode (OpenGL 1.3)
        GLenum combineModeAlpha = GL_MODULATE;
        GLenum combineModeRGB = GL_MODULATE;
        GLenum sourceAlpha[3] = { GL_TEXTURE, GL_PREVIOUS, GL_CONSTANT };
        GLenum sourceRGB[3] = { GL_TEXTURE, GL_PREVIOUS, GL_CONSTANT };
        GLenum operandAlpha[3] = { GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA };
        GLenum operandRGB[3] = { GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_ALPHA };
        char numArgsAlpha = 2;
        char numArgsRGB = 2;

        // Texture environment constant color
        float colorConstA = 0.0f;
        float colorConstR = 0.0f;
        float colorConstG = 0.0f;
        float colorConstB = 0.0f;

        // Color scaling
        float colorScaleA = 1.0f;
        float colorScaleRGB = 1.0f;
    };

    // Stores parameters of a texture object (set with glTexParameter())
    struct TextureParameter {

        bool isUsingMipMapping = true;
        bool isUsingTrilinearFilter = false;
        SamplerMethod minifySampler = &sampleTexelsLinear;
        SamplerMethod magnifySampler = &sampleTexelsLinear;

        GLenum wrappingModeS = GL_REPEAT;
        GLenum wrappingModeT = GL_REPEAT;

        GLenum minifyFilter = GL_NEAREST_MIPMAP_LINEAR;
        GLenum magnifyFilter = GL_LINEAR;

        // ...
    };

    // Stores the actual pixels of a texture
    struct TextureMipMap {

        int width;
        int height;
        TexturePixels pixel;
    };
    using TextureMipMaps = std::array<std::vector<TextureMipMap>, SWGL_MAX_TEXTURE_LOD + 1>;

    struct TextureData {

        TextureBaseFormat format;

        int maxLOD;
        TextureMipMaps mips;

        virtual ~TextureData() { }
        virtual void sampleTexels(TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &colorOut) = 0;
    };

    struct TextureData1D : public TextureData {

        void sampleTexels(TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &colorOut) override;
    };

    struct TextureData2D : public TextureData {

        void sampleTexels(TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &colorOut) override;
    };

    struct TextureData3D : public TextureData {

        void sampleTexels(TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &colorOut) override;
    };

    struct TextureDataCubeMap : public TextureData {

        void sampleTexels(TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &colorOut) override;
    };

    // Stores all the state of a texture
    struct TextureObject {

        GLuint name;
        GLenum target;
        TextureParameter parameter;
        TextureDataPtr data;
    };

    // Indices of a texture target
    enum TextureTargetIndex : unsigned int {

        Target1D = 3U,
        Target2D = 2U,
        Target3D = 1U,
        TargetCubeMap = 0U
    };

    // A texture target
    struct TextureTarget {

        TextureObjectPtr texObj = nullptr;
        bool isEnabled = false;
    };

    // A texture unit
    struct TextureUnit {

        // The four different texture targets of a texture unit
        // (0 = Cubemap, 1 = 3D, 2 = 2D, 3 = 1D) and a pointer to
        // the target with the highest priority
        TextureTarget targets[4];
        TextureTarget *currentTarget = nullptr;

        // Stores texture environment information that is used to blend
        // texture colors with incoming fragment colors
        TextureEnvironment texEnv;
    };

    // Texture coordinates
    struct TextureCoordinates {

        QFloat s;
        QFloat t;
        QFloat r;
    };

    // Color
    struct ARGBColor {

        QFloat a;
        QFloat r;
        QFloat g;
        QFloat b;
    };

    //
    // Implements the texture management functionality of swGL
    //
    class TextureManager {

    public:
        TextureManager();
        ~TextureManager() = default;

    public:
        bool loadTextureImage2D(GLenum target, GLint mipLevel, GLsizei width, GLsizei height, GLint border, TextureBaseFormat internalFormat, GLenum externalFormat, GLenum externalType, const GLvoid *pixels);
        bool loadSubTextureImage2D(GLenum target, GLint mipLevel, GLint x, GLint y, GLsizei width, GLsizei height, GLenum externalFormat, GLenum externalType, const GLvoid *pixels);

    private:
        bool readTextureData2D(GLsizei offsX, GLsizei offsY, GLsizei width, GLsizei height, TextureBaseFormat internalFormat, GLenum externalFormat, GLenum externalType, const GLvoid *src, unsigned int *dst);
        unsigned int convertFormat(TextureBaseFormat newFormat, unsigned char srcR, unsigned char srcG, unsigned char srcB, unsigned char srcA);

    public:
        bool bindTexture(GLenum target, GLuint name);
        void deleteTexture(GLuint name);
        void setActiveTextureUnit(unsigned int unitIdx);
        void setTargetEnable(GLenum target, bool isEnabled);

    public:
        bool isTextureResident(GLuint name);
        bool isTextureTargetBound(GLenum target);
        void getFreeTextureNames(GLint count, GLuint *names);
        TextureEnvironment &getActiveTextureEnvironment();
        TextureParameter &getTextureParameter(GLenum target);
        TextureUnit &getTextureUnit(unsigned int idx);
        bool getCompatibleFormat(GLenum desiredFormat, TextureBaseFormat &compatibleFormat);

    private:
        TextureObjectPtr createTextureObject(GLuint name, GLenum target);
        TextureObjectPtr getTextureObjectByName(GLuint name);
        TextureObjectPtr getTextureObjectByTarget(GLenum target, unsigned int &faceIdxOut);
        TextureTarget *getTextureTarget(TextureUnit *unit, GLenum target);

    private:
        TextureUnit m_unit[SWGL_MAX_TEXTURE_UNITS];
        TextureUnit *m_activeUnit;

    private:
        std::map<GLuint, TextureObjectPtr> m_textureObjects;
        std::deque<GLuint> m_freeTextures;
    };
}
