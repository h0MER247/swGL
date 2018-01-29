#include "TextureManager.h"
#include "Log.h"
#include "Context.h"

#if 0
// Just for debugging purposes
#include <string>
#include "TargaWriter.h"
namespace SWGL {

    static const char *formatToString(TextureBaseFormat format) {

        switch (format) {

        case TextureBaseFormat::Alpha: return "Alpha";
        case TextureBaseFormat::Intensity: return "Intensity";
        case TextureBaseFormat::Luminance: return "Luminance";
        case TextureBaseFormat::LuminanceAlpha: return "LuminanceAlpha";
        case TextureBaseFormat::RGB: return "RGB";
        case TextureBaseFormat::RGBA: return "RGBA";

        default:
            return "Invalid";
        }
    }

    static void dumpTexture(TextureObjectPtr &texObj, int mipLevel) {

        if (texObj != nullptr &&
            texObj->data->maxLOD >= mipLevel) {

            auto &texFaces = texObj->data->mips[mipLevel];

            for (auto i = 0U; i < texFaces.size(); i++) {

                auto &texMip = texFaces[i];

                if (texMip.width > 0 &&
                    texMip.height > 0 &&
                    texMip.pixel.size() > 0) {

                    writeTargaImage(

                        "tex" + std::to_string(texObj->name) + "_face" + std::to_string(i) + "_mip" + std::to_string(mipLevel) + "_fmt_" + formatToString(texObj->data->format),
                        texMip.pixel.data(),
                        texMip.width,
                        texMip.height
                    );
                }
            }
        }
    }
}
#endif

namespace SWGL {

    TextureManager::TextureManager() {

        m_activeUnit = &m_unit[0];
    }



    bool TextureManager::loadTextureImage2D(GLenum target, GLint mipLevel, GLsizei width, GLsizei height, GLint border, TextureBaseFormat internalFormat, GLenum externalFormat, GLenum externalType, const GLvoid *pixels) {

        unsigned int faceIdx;
        TextureObjectPtr texObj;

        if ((texObj = getTextureObjectByTarget(target, faceIdx)) != nullptr) {

            auto &texData = texObj->data;
            auto &texMip = texData->mips[mipLevel][faceIdx];
            auto &texPixel = texMip.pixel;

            // Synchronize with the drawing threads if this texture
            // is currently in use
            if (texData.use_count() > 0L) {

                SWGL::Context::getCurrentContext()->getRenderer().finish();
            }

            // Update max lod
            if (texData->maxLOD < mipLevel) {

                texData->maxLOD = mipLevel;
            }

            // Update format
            texData->format = internalFormat;

            // Update texture size
            if (width * height != static_cast<GLint>(texPixel.size())) {

                texPixel.resize(width * height);
                texMip.width = width;
                texMip.height = height;
            }

            // Read texture
            if (pixels != nullptr) {

                return readTextureData2D(0, 0, width, height, internalFormat, externalFormat, externalType, pixels, texPixel.data());
            }
            return true;
        }

        return false;
    }

    bool TextureManager::loadSubTextureImage2D(GLenum target, GLint mipLevel, GLint x, GLint y, GLsizei width, GLsizei height, GLenum externalFormat, GLenum externalType, const GLvoid *pixels) {

        unsigned int faceIdx;
        TextureObjectPtr texObj;

        if ((texObj = getTextureObjectByTarget(target, faceIdx)) != nullptr) {

            auto &texData = texObj->data;
            auto &texMip = texData->mips[mipLevel][faceIdx];
            auto &texPixel = texMip.pixel;
            auto &internalFormat = texData->format;

            // Synchronize with the drawing threads if this texture
            // is currently in use
            if (texData.use_count() > 0L) {

                SWGL::Context::getCurrentContext()->getRenderer().finish();
            }

            // Read texture
            if (pixels != nullptr) {

                return readTextureData2D(x, y, width, height, internalFormat, externalFormat, externalType, pixels, texPixel.data());
            }
            return true;
        }

        return false;
    }

    bool TextureManager::readTextureData2D(GLsizei offsX, GLsizei offsY, GLsizei width, GLsizei height, TextureBaseFormat internalFormat, GLenum externalFormat, GLenum externalType, const GLvoid *src, unsigned int *dst) {

        // TODO: Unpack alignment or other shenanigans aren't taken into account at the moment
        ptrdiff_t dstXOffs = 1;
        ptrdiff_t dstYOffs = dstXOffs * width;
        ptrdiff_t srcXOffs;
        ptrdiff_t srcYOffs;

        switch (externalFormat) {

        case GL_LUMINANCE:
            switch (externalType) {

            case GL_UNSIGNED_BYTE:
                srcXOffs = 1;
                srcYOffs = srcXOffs * width;
                for (int y = 0; y < height; y++) {

                    auto srcPix = reinterpret_cast<const unsigned char *>(src) + ((y + offsY) * srcYOffs) + (offsX * srcXOffs);
                    auto dstPix = reinterpret_cast<unsigned int *>(dst) + ((y + offsY) * dstYOffs) + (offsX * dstXOffs);

                    for (int x = 0; x < width; x++, srcPix += srcXOffs, dstPix += dstXOffs) {

                        *dstPix = convertFormat(internalFormat, srcPix[0], srcPix[0], srcPix[0], 0xff);
                    }
                }
                return true;

            default:
                LOG("Invalid or unimplemented external type %04x for format GL_ALPHA", externalType);
                return false;
            }
            break;

        case GL_ALPHA:
            switch (externalType) {

            case GL_UNSIGNED_BYTE:
                srcXOffs = 1;
                srcYOffs = srcXOffs * width;
                for (int y = 0; y < height; y++) {

                    auto srcPix = reinterpret_cast<const unsigned char *>(src) + ((y + offsY) * srcYOffs) + (offsX * srcXOffs);
                    auto dstPix = reinterpret_cast<unsigned int *>(dst) + ((y + offsY) * dstYOffs) + (offsX * dstXOffs);

                    for (int x = 0; x < width; x++, srcPix += srcXOffs, dstPix += dstXOffs) {

                        *dstPix = convertFormat(internalFormat, 0x00, 0x00, 0x00, srcPix[0]);
                    }
                }
                return true;

            default:
                LOG("Invalid or unimplemented external type %04x for format GL_ALPHA", externalType);
                return false;
            }
            break;

        case GL_BGR:
            switch (externalType) {

            case GL_UNSIGNED_BYTE:
                srcXOffs = 3;
                srcYOffs = srcXOffs * width;
                for (int y = 0; y < height; y++) {

                    auto srcPix = reinterpret_cast<const unsigned char *>(src) + ((y + offsY) * srcYOffs) + (offsX * srcXOffs);
                    auto dstPix = reinterpret_cast<unsigned int *>(dst) + ((y + offsY) * dstYOffs) + (offsX * dstXOffs);

                    for (int x = 0; x < width; x++, srcPix += srcXOffs, dstPix += dstXOffs) {

                        *dstPix = convertFormat(internalFormat, srcPix[2], srcPix[1], srcPix[0], 0xff);
                    }
                }
                return true;

            default:
                LOG("Invalid or unimplemented external type %04x for format GL_RGB", externalType);
                return false;
            }
            break;

        case GL_RGB:
            switch (externalType) {

            case GL_UNSIGNED_BYTE:
                srcXOffs = 3;
                srcYOffs = srcXOffs * width;
                for (int y = 0; y < height; y++) {

                    auto srcPix = reinterpret_cast<const unsigned char *>(src) + ((y + offsY) * srcYOffs) + (offsX * srcXOffs);
                    auto dstPix = reinterpret_cast<unsigned int *>(dst) + ((y + offsY) * dstYOffs) + (offsX * dstXOffs);

                    for (int x = 0; x < width; x++, srcPix += srcXOffs, dstPix += dstXOffs) {

                        *dstPix = convertFormat(internalFormat, srcPix[0], srcPix[1], srcPix[2], 0xff);
                    }
                }
                return true;

            default:
                LOG("Invalid or unimplemented external type %04x for format GL_RGB", externalType);
                return false;
            }
            break;

        case GL_RGBA:
            switch (externalType) {

            case GL_UNSIGNED_BYTE:
                srcXOffs = 4;
                srcYOffs = srcXOffs * width;
                for (int y = 0; y < height; y++) {

                    auto srcPix = reinterpret_cast<const unsigned char *>(src) + ((y + offsY) * srcYOffs) + (offsX * srcXOffs);
                    auto dstPix = reinterpret_cast<unsigned int *>(dst) + ((y + offsY) * dstYOffs) + (offsX * dstXOffs);

                    for (int x = 0; x < width; x++, srcPix += srcXOffs, dstPix += dstXOffs) {

                        *dstPix = convertFormat(internalFormat, srcPix[0], srcPix[1], srcPix[2], srcPix[3]);
                    }
                }
                return true;

            default:
                LOG("Invalid or unimplemented external type %04x for format GL_RGBA", externalType);
                return false;
            }
            break;

        default:
            LOG("Invalid or unimplemented external format: %04x", externalFormat);
            return false;
        }
    }

    unsigned int TextureManager::convertFormat(TextureBaseFormat newFormat, unsigned char srcR, unsigned char srcG, unsigned char srcB, unsigned char srcA) {

        #define BUILD_COLOR(R, G, B, A)             \
            (static_cast<unsigned int>(A) << 24) |  \
            (static_cast<unsigned int>(R) << 16) |  \
            (static_cast<unsigned int>(G) << 8)  |  \
             static_cast<unsigned int>(B)

        switch (newFormat) {

        case TextureBaseFormat::RGBA:
            return BUILD_COLOR(srcR, srcG, srcB, srcA);

        case TextureBaseFormat::Alpha:
            return BUILD_COLOR(0x00, 0x00, 0x00, srcA);

        case TextureBaseFormat::Intensity:
            return BUILD_COLOR(srcR, srcR, srcR, srcR);

        case TextureBaseFormat::Luminance:
            return BUILD_COLOR(srcR, srcR, srcR, 0xff);

        case TextureBaseFormat::LuminanceAlpha:
            return BUILD_COLOR(srcR, srcR, srcR, srcA);

        case TextureBaseFormat::RGB:
            return BUILD_COLOR(srcR, srcG, srcB, 0xff);

        default:
            LOG("Unimplemented texture base format");
            return BUILD_COLOR(0x00, 0x00, 0x00, 0x00);
        }

        #undef BUILD_COLOR
    }



    bool TextureManager::bindTexture(GLenum target, GLuint name) {

        auto texTarget = getTextureTarget(m_activeUnit, target);

        // Bind or unbind a texture to/from a given target
        if (name != 0U) {

            // Find the texture
            auto texObj = getTextureObjectByName(name);

            // Create a new texture object as it doesn't exist yet
            if (texObj == nullptr) {

                texTarget->texObj = createTextureObject(name, target);
                return true;
            }

            // Bind the texture if the targets match
            if (texObj->target == target) {

                texTarget->texObj = texObj;
                return true;
            }

            LOG("Texture binding failed!");
            return false;
        }
        else {

            // Unbind the current texture from the active texture unit
            texTarget->texObj = nullptr;
            return true;
        }
    }

    void TextureManager::deleteTexture(GLuint name) {

        auto it = m_textureObjects.find(name);
        if (it != m_textureObjects.end()) {

            auto &texObj = it->second;

            // Unbind this texture from all units
            if (texObj != nullptr) {

                for (auto i = 0U; i < SWGL_MAX_TEXTURE_UNITS; i++) {

                    auto texTarget = getTextureTarget(&m_unit[i], texObj->target);

                    if (texTarget->texObj != nullptr &&
                        texTarget->texObj->name == name) {

                        texTarget->texObj = nullptr;
                    }
                }
            }

            // Erase the texture
            m_textureObjects.erase(it);

            // The texture name is now available again
            m_freeTextures.push_back(name);
        }
    }

    void TextureManager::setActiveTextureUnit(unsigned int unitIdx) {

        m_activeUnit = &m_unit[unitIdx];
    }

    void TextureManager::setTargetEnable(GLenum target, bool isEnabled) {

        auto texTarget = getTextureTarget(m_activeUnit, target);
        texTarget->isEnabled = isEnabled;

        // Find the most "prioritized" texture target and make it the current
        // target. Cubemap has the highest priority, followed by 3D, 2D and 1D
        // textures.
        m_activeUnit->currentTarget = nullptr;

        for (auto &target : m_activeUnit->targets) {

            if (target.isEnabled) {

                m_activeUnit->currentTarget = &target;
                break;
            }
        }
    }



    bool TextureManager::isTextureResident(GLuint name) {

        return m_textureObjects.find(name) != m_textureObjects.end();
    }

    bool TextureManager::isTextureTargetBound(GLenum target) {

        return getTextureTarget(m_activeUnit, target)->texObj != nullptr;
    }

    void TextureManager::getFreeTextureNames(GLint count, GLuint *names) {

        for (int i = 0; i < count; i++) {

            // Generate free texture names if necessary
            if (m_freeTextures.empty()) {

                auto texName = 1U;
                for (auto count = 0U; count < 1024U; ) {

                    auto it = m_textureObjects.find(texName);
                    if (it == m_textureObjects.end()) {

                        m_freeTextures.push_back(texName);
                        count++;
                    }

                    texName++;
                }
            }

            // Get free texture name
            names[i] = m_freeTextures.front();
            m_freeTextures.pop_front();

            // Reserve the name
            m_textureObjects[names[i]] = nullptr;
        }
    }

    TextureEnvironment &TextureManager::getActiveTextureEnvironment() {

        return m_activeUnit->texEnv;
    }

    TextureParameter &TextureManager::getTextureParameter(GLenum target) {

        return getTextureTarget(m_activeUnit, target)->texObj->parameter;
    }

    TextureUnit &TextureManager::getTextureUnit(unsigned int idx) {

        return m_unit[idx];
    }

    bool TextureManager::getCompatibleFormat(GLenum desiredFormat, TextureBaseFormat &compatibleFormat) {

        // swGL doesn't care much about sized formats (at least for now).
        // Every component gets stored as a 8 bit integer value, regardless
        // of whether a sized format was given or not. Makes things easier
        // and is even allowed by the OpenGL specification.

        switch (desiredFormat) {

        case GL_ALPHA:
        case GL_ALPHA4:
        case GL_ALPHA8:
        case GL_ALPHA12:
        case GL_ALPHA16:
            compatibleFormat = TextureBaseFormat::Alpha;
            return true;

        case 1:
        case GL_LUMINANCE:
        case GL_LUMINANCE4:
        case GL_LUMINANCE8:
        case GL_LUMINANCE12:
        case GL_LUMINANCE16:
            compatibleFormat = TextureBaseFormat::Luminance;
            return true;

        case 2:
        case GL_LUMINANCE_ALPHA:
        case GL_LUMINANCE4_ALPHA4:
        case GL_LUMINANCE6_ALPHA2:
        case GL_LUMINANCE8_ALPHA8:
        case GL_LUMINANCE12_ALPHA4:
        case GL_LUMINANCE12_ALPHA12:
        case GL_LUMINANCE16_ALPHA16:
            compatibleFormat = TextureBaseFormat::LuminanceAlpha;
            return true;

        case GL_INTENSITY:
        case GL_INTENSITY4:
        case GL_INTENSITY8:
        case GL_INTENSITY12:
        case GL_INTENSITY16:
            compatibleFormat = TextureBaseFormat::Intensity;
            return true;

        case 3:
        case GL_RGB:
        case GL_R3_G3_B2:
        case GL_RGB4:
        case GL_RGB5:
        case GL_RGB8:
        case GL_RGB10:
        case GL_RGB12:
        case GL_RGB16:
        case GL_BGR:
            compatibleFormat = TextureBaseFormat::RGB;
            return true;

        case 4:
        case GL_RGBA:
        case GL_RGBA2:
        case GL_RGBA4:
        case GL_RGB5_A1:
        case GL_RGBA8:
        case GL_RGB10_A2:
        case GL_RGBA12:
        case GL_RGBA16:
        case GL_BGRA:
            compatibleFormat = TextureBaseFormat::RGBA;
            return true;

        default:
            LOG("Unable to derive a compatible base format for %04x", desiredFormat);
            return false;
        }
    }



    TextureObjectPtr TextureManager::createTextureObject(GLuint name, GLenum target) {

        auto texObj = std::make_shared<TextureObject>();
        int numFaces;

        switch (target) {

        case GL_TEXTURE_1D:
            texObj->data = std::make_shared<TextureData1D>();
            numFaces = 1;
            break;

        case GL_TEXTURE_2D:
            texObj->data = std::make_shared<TextureData2D>();
            numFaces = 1;
            break;

        case GL_TEXTURE_3D:
            texObj->data = std::make_shared<TextureData3D>();
            numFaces = 1; // TODO: 3D textures aren't implemented at the moment
            break;

        case GL_TEXTURE_CUBE_MAP:
            texObj->data = std::make_shared<TextureDataCubeMap>();
            numFaces = 6;
            break;

        default:
            LOG("Unimplemented target specified");
            return nullptr;
        }

        for (auto &texFace : texObj->data->mips) {

            texFace.resize(numFaces);
        }

        texObj->name = name;
        texObj->target = target;
        texObj->data->maxLOD = -1;

        return m_textureObjects[name] = texObj;
    }

    TextureObjectPtr TextureManager::getTextureObjectByName(GLuint name) {

        auto it = m_textureObjects.find(name);
        if (it != m_textureObjects.end()) {

            return it->second;
        }

        return nullptr;
    }

    TextureObjectPtr TextureManager::getTextureObjectByTarget(GLenum target, unsigned int &faceIdxOut) {

        if (m_activeUnit != nullptr) {

            switch (target) {

            case GL_TEXTURE_2D:
                faceIdxOut = 0U;
                return m_activeUnit->targets[TextureTargetIndex::Target2D].texObj;

            case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
            case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
            case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
            case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
            case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
            case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
                faceIdxOut = target - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
                return m_activeUnit->targets[TextureTargetIndex::TargetCubeMap].texObj;

            case GL_PROXY_TEXTURE_2D:
                LOG("Unimplemented GL_PROXY_TEXTURE_2D");
                return nullptr;

            case GL_PROXY_TEXTURE_CUBE_MAP:
                LOG("Unimplemented GL_PROXY_TEXTURE_CUBE_MAP");
                return nullptr;

            default:
                LOG("Invalid target: %04x", target);
                return nullptr;
            }
        }

        LOG("No active texture unit");
        return nullptr;
    }

    TextureTarget *TextureManager::getTextureTarget(TextureUnit *unit, GLenum target) {

        switch (target) {

        case GL_TEXTURE_1D:
            return &unit->targets[TextureTargetIndex::Target1D];

        case GL_TEXTURE_2D:
            return &unit->targets[TextureTargetIndex::Target2D];

        case GL_TEXTURE_3D:
            return &unit->targets[TextureTargetIndex::Target3D];

        case GL_TEXTURE_CUBE_MAP:
            return &unit->targets[TextureTargetIndex::TargetCubeMap];
        }

        LOG("Unimplemented texture target: %04x", target);
        return nullptr;
    }
}
