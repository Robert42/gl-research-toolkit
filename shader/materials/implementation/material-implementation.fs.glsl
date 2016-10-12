#include "input-block.fs.glsl"

#ifdef FORWARD_RENDERER
#include "forward-lighting.fs.glsl"
#endif

#ifdef DEFERRED_RENDERER
#include "deferred-lighting.fs.glsl"
#endif

#include <checkerboard.glsl>

