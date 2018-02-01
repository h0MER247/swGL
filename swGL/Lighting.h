#pragma once

#include <cmath>
#include <algorithm>
#include "Vector.h"
#include "Vertex.h"
#include "Matrix.h"

namespace SWGL {

    //
    // Implements OpenGL's lighting system
    //
    class Lighting {

    public:
        Lighting()

            : m_isEnabled(false),
              m_lightEnableMask(0),
              m_isTwoSided(false),
              m_isSeparateSpecularEnabled(false),
              m_isReflectionOriginEye(false),
              m_isColorMaterialEnabled(false),
              m_ambientSceneColor(Vector(0.2f, 0.2f, 0.2f, 1.0f)) {

            m_lights[0].diffuseIntensity = Vector(1.0f, 1.0f, 1.0f, 1.0f);
            m_lights[0].specularIntensity = Vector(1.0f, 1.0f, 1.0f, 1.0f);
            for (auto i = 1U; i < SWGL_MAX_LIGHTS; i++) {

                m_lights[i].diffuseIntensity = Vector(0.0f, 0.0f, 0.0f, 0.0f);
                m_lights[i].specularIntensity = Vector(0.0f, 0.0f, 0.0f, 0.0f);
            }

            setColorMaterialMode(true, true, GL_AMBIENT_AND_DIFFUSE);
        }
        ~Lighting() = default;

    public:
        void setLightingEnable(bool isEnabled) {

            m_isEnabled = isEnabled;
        }

        bool isEnabled() {

            return m_isEnabled && m_lightEnableMask != 0U;
        }

        void setTwoSidedLightingEnable(bool isTwoSided) {

            m_isTwoSided = isTwoSided;
        }

        void setAmbientSceneColor(const Vector &color) {

            m_ambientSceneColor = color;
        }

        void setSeparateSpecularColorEnable(bool isEnabled) {

            m_isSeparateSpecularEnabled = isEnabled;
        }

        void setReflectionCalculatedFromEyeOrigin(bool isComputedFromEyeOrigin) {

            m_isReflectionOriginEye = isComputedFromEyeOrigin;
        }

    public:
        void setLightEnable(unsigned int idx, bool isEnabled) {

            m_lights[idx].isEnabled = isEnabled;

            if (isEnabled) {

                m_lightEnableMask |= 1U << idx;
            }
            else {

                m_lightEnableMask &= ~(1U << idx);
            }
        }

        void setLightPosition(unsigned int idx, const Vector &position) {

            m_lights[idx].position = position;
        }

        void setLightSpotDirection(unsigned int idx, const Vector &direction) {

            m_lights[idx].spotDirection = direction;
            m_lights[idx].spotDirectionNormalized = Vector::normalize3(direction);
        }

        void setLightSpotExponent(unsigned int idx, float exponent) {

            m_lights[idx].spotExponent = exponent;
        }

        void setLightSpotCutOffValue(unsigned int idx, float cutOffValue) {

            m_lights[idx].spotCutOffValue = cutOffValue;
            m_lights[idx].spotCutOffValueCos = std::cos(cutOffValue * 3.1415926538f / 180.0f);
        }

        void setLightConstantAttenuation(unsigned int idx, float attenuation) {

            m_lights[idx].attenuationConstant = attenuation;
        }

        void setLightLinearAttenuation(unsigned int idx, float attenuation) {

            m_lights[idx].attenuationLinear = attenuation;
        }

        void setLightQuadraticAttenuation(unsigned int idx, float attenuation) {

            m_lights[idx].attenuationQuadratic = attenuation;
        }

        void setLightAmbientIntensity(unsigned int idx, const Vector &intensity) {

            m_lights[idx].ambientIntensity = intensity;
        }

        void setLightDiffuseIntensity(unsigned int idx, const Vector &intensity) {

            m_lights[idx].diffuseIntensity = intensity;
        }

        void setLightSpecularIntensity(unsigned int idx, const Vector &intensity) {

            m_lights[idx].specularIntensity = intensity;
        }

    public:
        void setMaterialAmbientColor(bool isFront, bool isBack, const Vector &color) {

            if (isFront) { m_material[0].ambientColor = color; }
            if (isBack) { m_material[1].ambientColor = color; }
        }

        void setMaterialDiffuseColor(bool isFront, bool isBack, const Vector &color) {

            if (isFront) { m_material[0].diffuseColor = color; }
            if (isBack) { m_material[1].diffuseColor = color; }
        }

        void setMaterialSpecularColor(bool isFront, bool isBack, const Vector &color) {

            if (isFront) { m_material[0].specularColor = color; }
            if (isBack) { m_material[1].specularColor = color; }
        }

        void setMaterialEmissionColor(bool isFront, bool isBack, const Vector &color) {

            if (isFront) { m_material[0].emissionColor = color; }
            if (isBack) { m_material[1].emissionColor = color; }
        }

        void setMaterialSpecularExponent(bool isFront, bool isBack, float exponent) {

            if (isFront) { m_material[0].specularExponent = exponent; }
            if (isBack) { m_material[1].specularExponent = exponent; }
        }

    public:
        void setColorMaterialEnable(bool isEnabled) {

            m_isColorMaterialEnabled = isEnabled;
            updateColorMaterial();
        }

        void setColorMaterialMode(bool isFront, bool isBack, GLenum mode) {

            auto &front = m_colorMaterial[0];
            auto &back = m_colorMaterial[1];

            bool trackAmbient = mode == GL_AMBIENT || mode == GL_AMBIENT_AND_DIFFUSE;
            bool trackDiffuse = mode == GL_DIFFUSE || mode == GL_AMBIENT_AND_DIFFUSE;
            bool trackEmission = mode == GL_EMISSION;
            bool trackSpecular = mode == GL_SPECULAR;

            if (isFront) {

                front.trackAmbient = trackAmbient;
                front.trackDiffuse = trackDiffuse;
                front.trackEmission = trackEmission;
                front.trackSpecular = trackSpecular;
            }
            if (isBack) {

                back.trackAmbient = trackAmbient;
                back.trackDiffuse = trackDiffuse;
                back.trackEmission = trackEmission;
                back.trackSpecular = trackSpecular;
            }

            updateColorMaterial();
        }

    private:
        void updateColorMaterial() {

            if (m_isColorMaterialEnabled) {

                for (auto &mat : m_colorMaterial) {

                    mat.isTrackingAmbient = mat.trackAmbient;
                    mat.isTrackingDiffuse = mat.trackDiffuse;
                    mat.isTrackingEmission = mat.trackEmission;
                    mat.isTrackingSpecular = mat.trackSpecular;
                }
            }
            else {

                for (auto &mat : m_colorMaterial) {

                    mat.isTrackingAmbient = false;
                    mat.isTrackingDiffuse = false;
                    mat.isTrackingEmission = false;
                    mat.isTrackingSpecular = false;
                }
            }
        }

    public:
        void calculateLighting(TriangleList &triangles) {

            // TODO: Specular light, two sided lighting, optimization, etc...

            for (auto &t : triangles) {

                for (auto &v : t.v) {

                    //
                    // Calculate lighting
                    //
                    auto &ambientColor = m_colorMaterial[0].isTrackingAmbient ? v.colorPrimary : m_material[0].ambientColor;
                    auto &diffuseColor = m_colorMaterial[0].isTrackingDiffuse ? v.colorPrimary : m_material[0].diffuseColor;
                    auto &emissionColor = m_colorMaterial[0].isTrackingEmission ? v.colorPrimary : m_material[0].emissionColor;
                    //auto &specularColor = m_colorMaterial[0].isTrackingSpecular ? v.colorPrimary : m_material[0].specularColor;

                    Vector vertexToLight;

                    float resultR = emissionColor.r() + (ambientColor.r() * m_ambientSceneColor.r());
                    float resultG = emissionColor.g() + (ambientColor.g() * m_ambientSceneColor.g());
                    float resultB = emissionColor.b() + (ambientColor.b() * m_ambientSceneColor.b());

                    for (auto i = 0U; i < SWGL_MAX_LIGHTS; i++) {

                        auto &light = m_lights[i];
                        if (!light.isEnabled) {

                            continue;
                        }

                        // Calculate attenuation factor
                        float lightAttenuation = 1.0f;
                        if (light.position.w() != 0.0f) {

                            vertexToLight = light.position - v.posEye;

                            float len = Vector::length3(vertexToLight);

                            vertexToLight *= 1.0f / len;
                            lightAttenuation /= light.attenuationConstant + len * (light.attenuationLinear + len * light.attenuationQuadratic);
                        }
                        else {

                            // ???
                            vertexToLight = Vector::normalize3(light.position);
                        }
                        if (light.spotCutOffValue != 180.0f) {

                            float spotFactor = -Vector::dot3(vertexToLight, light.spotDirectionNormalized);
                            if (spotFactor < light.spotCutOffValueCos) {

                                lightAttenuation = 0.0f;
                            }
                            else {

                                lightAttenuation *= std::pow(spotFactor, light.spotExponent);
                            }
                        }

                        // Ambient light
                        resultR += ambientColor.r() * light.ambientIntensity.r();
                        resultG += ambientColor.g() * light.ambientIntensity.g();
                        resultB += ambientColor.b() * light.ambientIntensity.b();

                        // Diffuse light
                        float diffuseFactor = Vector::dot3(vertexToLight, v.normal);
                        if (diffuseFactor > 0.0f) {

                            resultR += lightAttenuation * (diffuseFactor * diffuseColor.r() * light.diffuseIntensity.r());
                            resultG += lightAttenuation * (diffuseFactor * diffuseColor.g() * light.diffuseIntensity.g());
                            resultB += lightAttenuation * (diffuseFactor * diffuseColor.b() * light.diffuseIntensity.b());
                        }
                    }

                    // Set resulting colors
                    v.colorPrimary.a() = std::clamp(m_material[0].diffuseColor.a(), 0.0f, 1.0f);
                    v.colorPrimary.b() = std::clamp(resultB, 0.0f, 1.0f);
                    v.colorPrimary.g() = std::clamp(resultG, 0.0f, 1.0f);
                    v.colorPrimary.r() = std::clamp(resultR, 0.0f, 1.0f);
                    //v.colorSecondary.a() = 0.0f;
                    //v.colorSecondary.b() = 0.0f;
                    //v.colorSecondary.g() = 0.0f;
                    //v.colorSecondary.r() = 0.0f;
                }
            }
        }

    private:
        bool m_isEnabled;
        unsigned int m_lightEnableMask;
        bool m_isTwoSided;
        bool m_isSeparateSpecularEnabled;
        bool m_isReflectionOriginEye;
        bool m_isColorMaterialEnabled;

    private:
        Vector m_ambientSceneColor;
        struct Light {

            bool isEnabled = false;

            Vector position = Vector(0.0f, 0.0f, 1.0f, 0.0f);
            Vector spotDirection = Vector(0.0f, 0.0f, -1.0f, 0.0f);
            Vector spotDirectionNormalized = Vector(0.0f, 0.0f, -1.0f, 0.0f);
            float spotExponent = 0.0f;
            float spotCutOffValue = 180.0f;
            float spotCutOffValueCos = -1.0f;
            float attenuationConstant = 1.0f;
            float attenuationLinear = 0.0f;
            float attenuationQuadratic = 0.0f;
            Vector ambientIntensity = Vector(0.0f, 0.0f, 0.0f, 1.0f);
            Vector diffuseIntensity;
            Vector specularIntensity;

        } m_lights[SWGL_MAX_LIGHTS];

    private:
        struct Material {

            Vector ambientColor = Vector(0.2f, 0.2f, 0.2f, 1.0f);
            Vector diffuseColor = Vector(0.8f, 0.8f, 0.8f, 1.0f);
            Vector emissionColor = Vector(0.0f, 0.0f, 0.0f, 1.0f);
            Vector specularColor = Vector(0.0f, 0.0f, 0.0f, 1.0f);
            float specularExponent = 0.0f;

        } m_material[2];

    private:
        struct ColorMaterial {

            bool trackAmbient, isTrackingAmbient;
            bool trackDiffuse, isTrackingDiffuse;
            bool trackEmission, isTrackingEmission;
            bool trackSpecular, isTrackingSpecular;

        } m_colorMaterial[2];
    };
}
