#pragma once

#include <map>
#include <deque>
#include <array>
#include <vector>
#include <string>
#include <memory>
#include "Defines.h"
#include "SIMD.h"
#include "OpenGL.h"
#include "ContextTypes.h"
#include "AlignedAllocator.h"

namespace SWGL {

    // Forward declarations
    struct TextureObject;
    struct TextureParameter;
    struct TextureCoordinates;
    struct ARGBColor;

    // Type aliases
    using TextureObjectPtr = std::shared_ptr<TextureObject>;
    using SamplerMethod = void(*)(TextureObjectPtr &, TextureParameter &, int, TextureCoordinates &, ARGBColor &);

    // Texture sampling methods from TextureSampler.cpp
    extern void sampleTexelsNearest(TextureObjectPtr &tex, TextureParameter &texParams, int lod, TextureCoordinates &texCoords, ARGBColor &color);
    extern void sampleTexelsLinear(TextureObjectPtr &tex, TextureParameter &texParams, int lod, TextureCoordinates &texCoords, ARGBColor &color);
    extern void sampleTexels(TextureObjectPtr &tex, TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &color);



    // The different texture targets
    enum class TextureTargetID : size_t {

        Target1D,
        Target2D,
        Target3D,
        TargetCubeMap

        // ...
    };

    // This describes the format in which swGL stored a texture
    enum class TextureBaseFormat : size_t {

        Alpha,
        Luminance,
        LuminanceAlpha,
        Intensity,
        RGB,
        RGBA
    };

    // Stores environment data of a texture unit (set with glTexEnv())
    struct TextureEnvironment {

        GLenum mode = GL_MODULATE;

        float colorA = 0.0f;
        float colorR = 0.0f;
        float colorG = 0.0f;
        float colorB = 0.0f;
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
    struct TexturePixels {

        int width;
        int height;
        std::vector<unsigned int, AlignedAllocator<unsigned int, 16>> pixel;
    };

    // Stores all the state of a texture
    struct TextureObject {

        GLuint name;
        TextureTargetID target;
        TextureParameter parameter;
        TextureBaseFormat format;

        int maxLOD;
        std::array<TexturePixels, SWGL_MAX_TEXTURE_LOD> mips;
    };

    // A texture target
    struct TextureTarget {

        TextureObjectPtr texObj = nullptr;
        bool isEnabled = false;
    };

    // A texture unit
    struct TextureUnit {

        // The four different texture targets of a texture unit (Cubemap, 3D, 2D, 1D)
        // and a pointer to the target with the highest priority
        TextureTarget target1D;
        TextureTarget target2D;
        TextureTarget target3D;
        TextureTarget targetCubeMap;
        TextureTarget *currentTarget = nullptr;

        // Stores texture environment information that is used to blend texture colors
        // with incoming fragment colors
        TextureEnvironment texEnv;
    };

    // Texture coordinates
    struct TextureCoordinates {

        QFloat u;
        QFloat v;
        // ...
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
    // TODO: Refactor this class, it is a mess.
    //
    class TextureManager {

    public:
        TextureManager();
        ~TextureManager() = default;

    public:
        bool loadTextureTarget2D(GLint mipLevel, GLsizei width, GLsizei height, GLint border, TextureBaseFormat internalFormat, GLenum externalFormat, GLenum externalType, const GLvoid *pixels);
        bool loadSubTextureTarget2D(GLint mipLevel, GLint x, GLint y, GLsizei width, GLsizei height, GLenum externalFormat, GLenum externalType, const GLvoid *pixels);

    private:
        bool readTextureData2D(GLsizei offsX, GLsizei offsY, GLsizei width, GLsizei height, GLenum externalFormat, GLenum externalType, const GLvoid *src, TexturePixels &texPix);

    public:
        bool bindTexture(TextureTargetID target, GLuint name);
        void deleteTexture(GLuint name);
        bool containsTexture(GLuint name);
        void getFreeTextureNames(GLint count, GLuint *names);

        void setActiveTextureUnit(size_t unitIdx);
        void setTargetEnable(TextureTargetID target, bool isEnabled);
        bool isTextureTargetBound(TextureTargetID target);
        TextureEnvironment &getActiveTextureEnvironment();
        TextureParameter &getTextureParameter(TextureTargetID target);

        TextureUnit &getTextureUnit(size_t idx);

        bool getCompatibleFormat(GLenum desiredFormat, TextureBaseFormat &compatibleFormat);
        bool getTextureTargetID(GLenum target, TextureTargetID &targetID);

    private:
        TextureObjectPtr createTextureObject(GLuint name, TextureTargetID target);
        TextureObjectPtr getTextureObjectByName(GLuint name);
        TextureTarget &getTextureTarget(TextureUnit *unit, TextureTargetID target);

    private:
        TextureUnit m_unit[SWGL_MAX_TEXTURE_UNITS];
        TextureUnit *m_activeUnit;

    private:
        std::map<GLuint, TextureObjectPtr> m_textureObjects;

    private:
        std::deque<GLuint> m_freeTextures;
        void generateFreeTextureNames();
    };
}
