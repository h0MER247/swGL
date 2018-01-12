#pragma once

namespace SWGL {

    //
    // Implements a software based rgb gamma correction table
    //
    class GammaRamp {

    public:
        GammaRamp() {

            for (int i = 0; i < 256; i++) {

                m_red[i] = static_cast<char>(i);
                m_green[i] = static_cast<char>(i);
                m_blue[i] = static_cast<char>(i);
            }
        }
        ~GammaRamp() = default;

    public:
        void set(unsigned short *ramp) {

            if (ramp != nullptr) {

                for (int i = 0; i < 256; i++) {

                    m_red[i] = static_cast<char>((ramp[i] >> 8) & 0xff);
                    m_green[i] = static_cast<char>((ramp[i + 256] >> 8) & 0xff);
                    m_blue[i] = static_cast<char>((ramp[i + 512] >> 8) & 0xff);
                }
            }
        }

        void get(unsigned short *ramp) {

            if (ramp != nullptr) {

                for (int i = 0; i < 256; i++) {

                    ramp[i] = static_cast<unsigned short>(m_red[i] << 8);
                    ramp[i + 256] = static_cast<unsigned short>(m_green[i] << 8);
                    ramp[i + 512] = static_cast<unsigned short>(m_blue[i] << 8);
                }
            }
        }

    public:
        void correct(unsigned int *color, int count) {

            for (int i = 0; i < count; i++) {

                int r = (color[i] >> 16) & 0xff;
                int g = (color[i] >> 8) & 0xff;
                int b = color[i] & 0xff;

                color[i] = 0xff000000 | (m_red[r] << 16) | (m_green[g] << 8) | m_blue[b];
            }
        }

    private:
        unsigned char m_red[256];
        unsigned char m_green[256];
        unsigned char m_blue[256];
    };
}
