#pragma once

#include <fstream>
#include <string>
#include <stdint.h>
#include "Defines.h"

namespace SWGL {
    
    PACKED(
    
        struct TARGAFILEHEADER {

            int8_t idLength;
            int8_t colorMapType;
            int8_t imageType;

            int16_t colorMapFirstIdx;
            int16_t colorMapLength;
            int8_t colorMapBPP;

            int16_t imageOriginX;
            int16_t imageOriginY;
            int16_t imageWidth;
            int16_t imageHeight;
            int8_t imageBPP;
            int8_t imageDescriptor;
        }
    );

    static void writeTargaImage(std::string filename, unsigned int *data, int width, int height) {

        TARGAFILEHEADER header;
        header.idLength = 0;
        header.colorMapType = 0;
        header.imageType = 2;
        header.colorMapFirstIdx = 0;
        header.colorMapLength = 0;
        header.colorMapBPP = 0;
        header.imageOriginX = 0;
        header.imageOriginY = 0;
        header.imageWidth = width;
        header.imageHeight = height;
        header.imageBPP = 32;
        header.imageDescriptor = 0x27;

        std::fstream out(filename + ".tga", std::ios::out | std::ios::binary);
        out.write(reinterpret_cast<const char *>(&header), sizeof(TARGAFILEHEADER));
        out.write(reinterpret_cast<const char *>(data), width * height * 4 * sizeof(char));
        out.close();
    }
}
