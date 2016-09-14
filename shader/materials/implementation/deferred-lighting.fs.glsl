#ifdef DEFERRED_RENDERER

#include <lighting/rendering-equation.glsl>

layout(location=0) out vec4 worldNormal_normalLength;
layout(location=1) out vec4 meshNormal_meshBiTangentX;
layout(location=2) out vec4 meshTangent_meshBiTangentY;
layout(location=3) out float meshBiTangentZ;
layout(location=4) out vec4 baseColor_metalMask;
layout(location=5) out vec4 emission_reflectance;
layout(location=6) out vec2 occlusion_smoothness;


void apply_material(in BaseMaterial material, in SurfaceData surface, mat3 tangent_to_worldspace, float alpha)
{
  worldNormal_normalLength = vec4(material.normal, material.normal_length);
  meshNormal_meshBiTangentX = vec4(tangent_to_worldspace[2], tangent_to_worldspace[1].x);
  meshTangent_meshBiTangentY = vec4(tangent_to_worldspace[0], tangent_to_worldspace[1].y);
  meshBiTangentZ = tangent_to_worldspace[1].z;
  baseColor_metalMask = vec4(material.base_color, material.metal_mask);
  emission_reflectance = vec4(material.emission, material.reflectance);
  occlusion_smoothness = vec2(material.occlusion, material.smoothness);
}

#endif
