#pragma once

#include <vector>
#include "Vector.h"

namespace SWGL {

    // Forward declarations
    struct Vertex;

    // Type aliases
    using VertexList = std::vector<Vertex>;

    //
    // Holds the state of a vertex
    //
    struct Vertex {

        Vertex() = default;
        ~Vertex() = default;
        
        // Position
        Vector obj;
        Vector proj;
        Vector raster;

        // Varyings
        Vector color;
        Vector texCoord[SWGL_MAX_TEXTURE_UNITS];
    };
}
