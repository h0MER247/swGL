#include "TextureManager.h"
#if 0
// Just for debugging purposes
#include "TargaWriter.h"
namespace SWGL {

    static void dumpTexture(TextureObjectPtr &texObj, int mipLevel) {

        if (mipLevel > texObj->maxLOD)
            return;

        if (texObj->mips[mipLevel].width <= 0 || texObj->mips[mipLevel].height <= 0)
            return;

        writeTargaImage(

            "tex" + std::to_string(texObj->name) + "_mip" + std::to_string(mipLevel),
            texObj->mips[mipLevel].pixel.data(),
            texObj->mips[mipLevel].width,
            texObj->mips[mipLevel].height
        );
    }
}
#endif

namespace SWGL {

    TextureManager::TextureManager() {

        m_activeUnit = &m_unit[0];
    }



    bool TextureManager::loadTextureTarget2D(GLint mipLevel, GLsizei width, GLsizei height, GLint border, TextureBaseFormat internalFormat, GLenum externalFormat, GLenum externalType, const GLvoid *pixels) {

        auto &texObj = m_activeUnit->target2D.texObj;
        if (texObj == nullptr) {

            return false;
        }
        if (texObj->maxLOD < mipLevel) {

            texObj->maxLOD = mipLevel;
        }

        texObj->format = internalFormat;

        auto &mip = texObj->mips[mipLevel];
        mip.pixel.resize(width * height);
        mip.width = width;
        mip.height = height;

        return readTextureData2D(0, 0, width, height, externalFormat, externalType, pixels, mip);
    }

    bool TextureManager::loadSubTextureTarget2D(GLint mipLevel, GLint x, GLint y, GLsizei width, GLsizei height, GLenum externalFormat, GLenum externalType, const GLvoid *pixels) {
        
        auto &texObj = m_activeUnit->target2D.texObj;
        if (texObj == nullptr) {

            return false;
        }

        return readTextureData2D(x, y, width, height, externalFormat, externalType, pixels, texObj->mips[mipLevel]);
    }

    bool TextureManager::readTextureData2D(GLsizei offsX, GLsizei offsY, GLsizei width, GLsizei height, GLenum externalFormat, GLenum externalType, const GLvoid *src, TexturePixels &texPix) {

        // TODO: Unpack alignment or other shenanigans aren't taken into account at the moment
        size_t dstXOffs = 4;
        size_t dstYOffs = dstXOffs * width;
        size_t srcXOffs;
        size_t srcYOffs;

        auto dst = texPix.pixel.data();
        switch (externalFormat) {

        case GL_RGB:
            switch (externalType) {

            case GL_UNSIGNED_BYTE:
                srcXOffs = 3;
                srcYOffs = srcXOffs * width;
                for (int y = 0; y < height; y++) {

                    auto *srcPix = reinterpret_cast<const unsigned char *>(src) + ((y + offsY) * srcYOffs) + (offsX * srcXOffs);
                    auto *dstPix = reinterpret_cast<unsigned char *>(dst) + ((y + offsY) * dstYOffs) + (offsX * dstXOffs);

                    for (int x = 0; x < width; x++, srcPix += srcXOffs, dstPix += dstXOffs) {

                        dstPix[0] = srcPix[2];
                        dstPix[1] = srcPix[1];
                        dstPix[2] = srcPix[0];
                        dstPix[3] = 0xff;
                    }
                }
                return true;

            default:
                LOG("Invalid or unimplemented external type for GL_RGB format: %04x", externalType);
                return false;
            }
            break;

        case GL_RGBA:
            switch (externalType) {

            case GL_UNSIGNED_BYTE:
                srcXOffs = 4;
                srcYOffs = srcXOffs * width;
                for (int y = 0; y < height; y++) {

                    auto *srcPix = reinterpret_cast<const unsigned char *>(src) + ((y + offsY) * srcYOffs) + (offsX * srcXOffs);
                    auto *dstPix = reinterpret_cast<unsigned char *>(dst) + ((y + offsY) * dstYOffs) + (offsX * dstXOffs);

                    for (int x = 0; x < width; x++, srcPix += srcXOffs, dstPix += dstXOffs) {

                        dstPix[0] = srcPix[2];
                        dstPix[1] = srcPix[1];
                        dstPix[2] = srcPix[0];
                        dstPix[3] = srcPix[3];
                    }
                }
                return true;

            default:
                LOG("Invalid or unimplemented external type for GL_RGBA format: %04x", externalType);
                return false;
            }
            break;

        default:
            LOG("Invalid or unimplemented external format: %04x", externalFormat);
            return false;
        }
    }



    bool TextureManager::bindTexture(TextureTargetID target, GLuint name) {

        TextureTarget &texTarget = getTextureTarget(m_activeUnit, target);

        // Bind or unbind a texture to/from a given target
        if (name != 0U) {

            // Find the texture or create it if it doesn't exit yet
            TextureObjectPtr texObj = getTextureObjectByName(name);
            if (texObj == nullptr) {

                texObj = createTextureObject(name, target);
            }

            // Bind the texture if the targets match
            if (texObj->target == target) {

                texTarget.texObj = texObj;
                return true;
            }

            return false;
        }
        else {

            // Unbind the current texture from the active texture unit
            texTarget.texObj = nullptr;
            return true;
        }
    }

    void TextureManager::deleteTexture(GLuint name) {

        if (name == 0U) {

            return;
        }

        LOG ("Delete texture %d", name);

        auto it = m_textureObjects.find(name);
        if (it != m_textureObjects.end()) {

            auto &texObj = it->second;

            // Unbind this texture from all units
            for (int i = 0; i < SWGL_MAX_TEXTURE_UNITS; i++) {

                auto &texTarget = getTextureTarget(&m_unit[i], texObj->target);
                if(texTarget.texObj == texObj) {

                    texTarget.texObj = nullptr;
                }
            }

            // Erase the texture
            m_textureObjects.erase(it);

            // The texture name is now available again
            m_freeTextures.push_back(name);
        }
    }

    bool TextureManager::containsTexture(GLuint name) {

        return m_textureObjects.find(name) != m_textureObjects.end();
    }

    void TextureManager::getFreeTextureNames(GLint count, GLuint *names) {

        for (int i = 0; i < count; i++) {

            if (m_freeTextures.empty()) {

                generateFreeTextureNames();
            }

            names[i] = m_freeTextures.front();
            m_freeTextures.pop_front();
        }
    }



    void TextureManager::setActiveTextureUnit(size_t unitIdx) {

        m_activeUnit = &m_unit[unitIdx];
    }

    TextureEnvironment &TextureManager::getActiveTextureEnvironment() {

        return m_activeUnit->texEnv;
    }

    TextureParameter &TextureManager::getTextureParameter(TextureTargetID target) {

        return getTextureTarget(m_activeUnit, target).texObj->parameter;
    }

    TextureUnit &TextureManager::getTextureUnit(size_t idx) {

        return m_unit[idx];
    }

    void TextureManager::setTargetEnable(TextureTargetID target, bool isEnabled) {

        TextureTarget &texTarget = getTextureTarget(m_activeUnit, target);
        texTarget.isEnabled = isEnabled;

        // Find the most "prioritized" texture target and make it the current
        // target. Cubemap has the highest priority, follwed by 3D, 2D and 1D
        // textures.
        m_activeUnit->currentTarget = nullptr;

        if(m_activeUnit->targetCubeMap.isEnabled)
            m_activeUnit->currentTarget = &m_activeUnit->targetCubeMap;
        else if(m_activeUnit->target3D.isEnabled)
            m_activeUnit->currentTarget = &m_activeUnit->target3D;
        else if (m_activeUnit->target2D.isEnabled)
            m_activeUnit->currentTarget = &m_activeUnit->target2D;
        else if (m_activeUnit->target1D.isEnabled)
            m_activeUnit->currentTarget = &m_activeUnit->target1D;
    }

    bool TextureManager::isTextureTargetBound(TextureTargetID target) {

        return getTextureTarget(m_activeUnit, target).texObj != nullptr;
    }



    bool TextureManager::getCompatibleFormat(GLenum desiredFormat, TextureBaseFormat &compatibleFormat) {

        // swGL doesn't care much about sized formats. Every component
        // gets stored as a 8 bit integer value, regardless of whether
        // a sized format was given or not. Makes things easier and is
        // even allowed by the OpenGL specification.

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
            compatibleFormat = TextureBaseFormat::RGBA;
            return true;

        default:
            LOG("Invalid or unimplemented desired internal texture format: %04x", desiredFormat);
            return false;
        }
    }

    void TextureManager::generateFreeTextureNames() {

        // I know that this implementation sucks :D
        int count = 1024, texName = 0;

        while (count > 0) {

            auto it = m_textureObjects.find(texName);
            if (it == m_textureObjects.end()) {

                m_freeTextures.push_back(texName);
                count--;
            }

            texName++;
        }
    }



    bool TextureManager::getTextureTargetID(GLenum target, TextureTargetID &targetID) {

        switch (target) {

            case GL_TEXTURE_1D:
                targetID = TextureTargetID::Target1D;
                return true;

            case GL_TEXTURE_2D:
                targetID = TextureTargetID::Target2D;
                return true;

            case GL_TEXTURE_3D:
                targetID = TextureTargetID::Target3D;
                return true;

            case GL_TEXTURE_CUBE_MAP:
                targetID = TextureTargetID::TargetCubeMap;
                return true;

            default:
                return false;
        }
    }

    TextureTarget &TextureManager::getTextureTarget(TextureUnit *unit, TextureTargetID target) {

        switch (target) {

            case TextureTargetID::Target1D:
                return unit->target1D;

            case TextureTargetID::Target2D:
                return unit->target2D;

            case TextureTargetID::Target3D:
                return unit->target3D;

            default:
                return unit->targetCubeMap;
        }
    }

    TextureObjectPtr TextureManager::createTextureObject(GLuint name, TextureTargetID target) {

        auto texObj = std::make_shared<TextureObject>();
        texObj->name = name;
        texObj->target = target;
        texObj->maxLOD = -1;

        m_textureObjects[name] = texObj;

        return texObj;
    }

    TextureObjectPtr TextureManager::getTextureObjectByName(GLuint name) {

        auto it = m_textureObjects.find(name);
        if (it != m_textureObjects.end()) {

            return it->second;
        }

        return nullptr;
    }
}
