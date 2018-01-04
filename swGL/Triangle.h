#pragma once

#include <vector>
#include "Vertex.h"

namespace SWGL {

    // Forward declarations
    struct Triangle;

    // Type aliases
    using TriangleList = std::vector<Triangle>;

    //
    // Holds the state of a triangle
    //
    struct Triangle {

        Triangle() = default;
        ~Triangle() = default;

        Triangle(Vertex &v1, Vertex &v2, Vertex &v3)
        
            : v{v1, v2, v3} {

        }
        
        Vertex v[3];
    };
}
