#ifdef FORWARD_RENDERER

#include <lighting/rendering-equation.glsl>
#include <debugging/normal.glsl>

#include <glrt/glsl/layout-constants.h>

#include <scene/uniforms.glsl>

out vec4 fragment_color;

mat3 tangent_to_worldspace;
void calc_tangent_to_worldspace();

#include "highlight-numeric-issues.glsl"

void apply_material(in BaseMaterial material, in SurfaceData surface, float alpha)
{
  vec3 world_pos = surface.position;
  
  init_cone_bouquet(tangent_to_worldspace, world_pos);
  
#ifdef MASKED
  alpha = step(MASK_THRESHOLD, alpha);
#endif

#ifdef LIGHTING_FLAT
  fragment_color = vec4(material.base_color + material.emission, alpha);
  return;
#endif

#if defined(MATERIAL_NORMAL_WS)
  fragment_color = vec4(encode_direction_as_color(material.normal), alpha);
  return;
#elif defined(MATERIAL_ALPHA)
  fragment_color = vec4(vec3(alpha), 1);
  return;
#elif defined(MATERIAL_SMOOTHNESS)
  fragment_color = vec4(vec3(material.smoothness), alpha);
  return;
#elif defined(MATERIAL_BASE_COLOR)
  fragment_color = vec4(material.base_color, alpha);
  return;
#elif defined(MATERIAL_METAL_MASK)
  fragment_color = vec4(vec3(material.metal_mask), alpha);
return;
#elif defined(MATERIAL_EMISSION)
  fragment_color = vec4(material.emission, alpha);
  return;
#elif defined(MATERIAL_REFLECTANCE)
  fragment_color = vec4(vec3(material.reflectance), alpha);
  return;
#elif defined(MATERIAL_OCCLUSION)
  fragment_color = vec4(vec3(material.occlusion), alpha);
  return;
#endif

#if defined(BVH_NEAREST_LEAF_INDEX_0)
#define BVH_NEAREST_LEAF_INDEX_ 0
#elif defined(BVH_NEAREST_LEAF_INDEX_1)
#define BVH_NEAREST_LEAF_INDEX_ 1
#elif defined(BVH_NEAREST_LEAF_INDEX_2)
#define BVH_NEAREST_LEAF_INDEX_ 2
#endif
#if defined(BVH_OCCLUSION_GRID_ONLY_0)
#define BVH_OCCLUSION_GRID_ONLY_ 0
#elif defined(BVH_OCCLUSION_GRID_ONLY_1)
#define BVH_OCCLUSION_GRID_ONLY_ 1
#elif defined(BVH_OCCLUSION_GRID_ONLY_2)
#define BVH_OCCLUSION_GRID_ONLY_ 2
#endif

#if defined(BVH_NEAREST_LEAF_INDEX_)
  uvec4 nearest_leaves_index = texelFetch(cascaded_grid_texture(BVH_NEAREST_LEAF_INDEX_), ivec3(round(cascaded_grid_cell_from_worldspace(world_pos, BVH_NEAREST_LEAF_INDEX_))), 0);
  #ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  fragment_color = vec4(nearest_leaves_index.xyz / float(BVH_MAX_VISITED_LEAVES-1), 1);
  #else
  fragment_color = heatvision_linear(float(nearest_leaves_index[0]) / float(BVH_MAX_VISITED_LEAVES-1));
  #endif
  return;
#elif defined(BVH_OCCLUSION_GRID_ONLY_)
  float grid_occlusion = textureLod(cascaded_grid_texture_occlusion(BVH_OCCLUSION_GRID_ONLY_), cascaded_grid_uvw_from_worldspace(world_pos, BVH_OCCLUSION_GRID_ONLY_), 0).r;
  fragment_color = vec4(vec3(grid_occlusion), 1);
  return;
#elif defined(BVH_OCCLUSION_GRID)
  vec4 weights = cascadedGridWeights(world_pos);
  float grid_occlusion = merged_cascaded_grid_texture_occlusion(world_pos);
  PRINT_VALUE(weights);
  fragment_color = vec4(vec3(grid_occlusion), 1);
  return;
#endif

#if defined(CASCADED_GRID_WEIGHTS)
  fragment_color = vec4(cascadedGridWeights(surface.position).xyz, 1);
  return;
#elif defined(CASCADED_GRID_WEIGHTS_TINTED)
  fragment_color = vec4(cascadedGridWeights(surface.position).xyz * (0.75 + 0.25*length(material.base_color)), 1);
  return;
#endif

#if defined(MESH_NORMALS_WS)
  fragment_color = vec4(encode_direction_as_color(fragment.normal), 1);
  return;
#elif defined(MESH_UVS)
  fragment_color = vec4(fragment.uv, 0, 1);
  return;
#elif defined(MESH_TANGENTS_WS)
  fragment_color = vec4(encode_direction_as_color(fragment.tangent), 1);
  return;
#elif defined(MESH_BITANGENTS_WS)
  fragment_color = vec4(encode_direction_as_color(fragment.bitangent), 1);
  return;
#endif

#if defined(DISTANCEFIELD_AO) || defined(DISTANCEFIELD_AO_COST_TEX) || defined(DISTANCEFIELD_AO_COST_BRANCHING) || defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
  float ao_distancefield = distancefield_ao();
#if defined(DISTANCEFIELD_AO)
  fragment_color = vec4(vec3(ao_distancefield), 1);
#else
  fragment_color = heatvision(ao_distancefield_cost);
#endif
  return;
#endif

#if defined(AMBIENT_OCCLUSION)
  float ao_distancefield = distancefield_ao();
  float ao = material.occlusion * ao_distancefield;
  fragment_color = vec4(vec3(ao), 1);
  return;
#endif

  vec3 incoming_luminance = light_material(material, surface.position, scene.camera_position);
  
  float exposure = 1.f; // Only dummy value, to be corrected
  
  incoming_luminance *= exposure;

  fragment_color = vec4(accurateLinearToSRGB(incoming_luminance), alpha);
  
#if DEBUG_FRAGMENT_COLOR
  fragment_color = vec4(debug_fragment_color, alpha);
#endif

  highlight_numeric_issues();
}


#endif // FORWARD_RENDERER
