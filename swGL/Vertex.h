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

        // Position
        Vector posObj;
        Vector posEye;
        Vector posProj;

        // Normal
        Vector normal;

        // Varyings
        Vector colorPrimary;
        Vector colorSecondary;
        Vector texCoord[SWGL_MAX_TEXTURE_UNITS];
    };
}
