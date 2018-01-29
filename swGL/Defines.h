#pragma once

#define SWGLAPI extern "C"
#define STDCALL __stdcall
#define ADDRESS_OF(X) reinterpret_cast<const void *>(&X)

#ifdef __MINGW32__
#define INLINED __attribute__((always_inline)) inline
#define PACKED(decl) decl __attribute__((__packed__))
#define _aligned_malloc __mingw_aligned_malloc
#define _aligned_free __mingw_aligned_free
#else
#define INLINED __forceinline
#define PACKED(decl) __pragma(pack(push, 1)) decl __pragma(pack(pop))
#endif

// Enables avx2 instructions to gather texture samples (if avx2 is available).
#define SWGL_USE_AVX2_GATHER 0

// Whether swGL should use hardware gamma correction, or not
#define SWGL_USE_HARDWARE_GAMMA 1

// Enable / disable OpenGL call logging
#define SWGL_ENABLE_LOGGING 0

// Enable / disable log file flushing
#define SWGL_ENABLE_LOG_FLUSH 0

// The maximum number of matrices for one stack
static constexpr unsigned int SWGL_MAX_MATRIXSTACK_DEPTH = 32U;

// The total number of texture units (Doom 3 runs a little bit better with 4)
static constexpr unsigned int SWGL_MAX_TEXTURE_UNITS = 2U;

// Maximum level of detail for a texture
static constexpr unsigned int SWGL_MAX_TEXTURE_LOD = 10U;

// Maximum size of a texture given the maximum lod defined above
static constexpr unsigned int SWGL_MAX_TEXTURE_SIZE = 1U << SWGL_MAX_TEXTURE_LOD;

// Maximum number of user defined clipping planes
static constexpr unsigned int SWGL_MAX_CLIP_PLANES = 6U;

// Maximum number of lights
static constexpr unsigned int SWGL_MAX_LIGHTS = 8U;

// Number of drawing threads
static constexpr unsigned int SWGL_NUM_DRAW_THREADS = 16U;

// Maximum number of commands in the command queue of a drawing thread
static constexpr unsigned int SWGL_COMMAND_QUEUE_SIZE = 64U;

// Returns true if a given integer is a power of two
template<typename T>
static constexpr bool isPowerOfTwo(T value) {

    return (value & (value - 1)) == 0;
}

// Some compile time error checks
static_assert(isPowerOfTwo(SWGL_NUM_DRAW_THREADS), "The number of drawing threads has to be a power of two");
static_assert(SWGL_MAX_MATRIXSTACK_DEPTH >= 32U, "The matrix stack depth has to be at least 32");
static_assert(SWGL_MAX_TEXTURE_UNITS >= 2U, "The number of texture units has to be at least 2");
static_assert(SWGL_MAX_LIGHTS >= 8U, "The number of lights has to be at least 8");
static_assert(SWGL_MAX_CLIP_PLANES >= 6U, "The number of user defined clipping planes has to be at least 6");
