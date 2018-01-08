#pragma once

#include <array>
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
        std::array<Vector, SWGL_MAX_TEXTURE_UNITS> texCoord;
    };
}
