#define saturate(value) clamp(value, 0.f, 1.f)

#include "roughness-adaptation.glsl"
#include "structs.glsl"
#include "brdf.glsl"
#include "material.glsl"
#include "light.glsl"
#include "../mrp/mrp.glsl"
#include "luminance-for-luminous-power.glsl"
#include "camera.glsl"
#include "image-based-lighting.glsl"
#include "ld-prefiltering.glsl"

#undef saturate
