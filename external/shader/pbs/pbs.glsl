#define saturate(value) clamp(value, 0.f, 1.f)

#include "structs.glsl"
#include "brdf.glsl"
#include "material.glsl"
#include "light.glsl"
#include "../mrp/mrp.glsl"
#include "camera.glsl"


#undef saturate
