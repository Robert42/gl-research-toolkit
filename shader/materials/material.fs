#version 450 core
#include <extensions/command_list.glsl>

#if (defined(DEPTH_PREPASS) || defined(GBUFFER_FILL_PASS)) &&  defined(MASKED)
// TODO test, how this affects performance. Would performance be improved, if this layout woul dbe active always?
// https://www.opengl.org/registry/specs/ARB/conservative_depth.txt
layout(depth_unchanged) out float gl_FragDepth;
#else
layout(early_fragment_tests) in;
#endif

#include "implementation/material-implementation.fs.glsl"

mat3 tangent_to_worldspace;
void calc_tangent_to_worldspace();

#include <glrt/glsl/layout-constants.h>


#if defined(DEPTH_PREPASS) && defined(OPAQUE)

void main()
{
  return;
}

#elif defined(PLAIN_COLOR) || defined(AREA_LIGHT)

flat in BaseMaterial plainColorMaterial;

void main()
{
#if defined(TEXTURE_BASECOLOR) || defined(TEXTURE_BASECOLOR_ALPHA) || defined(TEXTURE_NORMAL_LS) || defined(TEXTURE_BUMP) || defined(TEXTURE_SMOOTHENESS) || defined(TEXTURE_REFLECTIVITY) || defined(TEXTURE_METALLIC) || defined(TEXTURE_AO) || defined(TEXTURE_EMISSION)
  fragment_color = checkerboard();
  return;
#endif

  calc_tangent_to_worldspace();
  
  SurfaceData surface;
  surface.position = fragment.position;
  
  apply_material(plainColorMaterial, surface, tangent_to_worldspace, 1.f);
}

#elif defined(TEXTURED)

layout(binding=UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, std140) uniform MaterialInstanceBlock
{
vec4 tint;
vec4 srmo_range_0;
vec4 srmo_range_1;
float emission_factor;
// sampler2D within Uniform Block possible thanks to GL_NV_bindless_texture
sampler2D basecolor_map;
sampler2D normal_map;
sampler2D height_map;
sampler2D srmo_map; // smoothness, reflectance, metalic_map, occlusion
sampler2D emission_map;
}material_instance;



void calculate_material_output(out BaseMaterial material, out SurfaceData surface, out float alpha)
{
  vec2 uv = fragment.uv;
  
  float height = texture2D(material_instance.height_map, uv).r;
  // TODO simple parallax mapping using height_map
  
  vec4 color = texture2D(material_instance.basecolor_map, uv) * material_instance.tint;
#ifndef DEPTH_PREPASS
  vec3 normal = texture2D(material_instance.normal_map, uv).xyz;
  vec4 srmo = texture2D(material_instance.srmo_map, uv);
  vec3 emission = texture2D(material_instance.emission_map, uv).rgb;

  #if defined(TEXTURE_BASECOLOR)
    fragment_color = color;
    return;
  #elif defined(TEXTURE_BASECOLOR_ALPHA)
    fragment_color = vec4(color.aaa, 1);
    return;
  #elif defined(TEXTURE_NORMAL_LS)
    fragment_color = vec4(encode_direction_as_color(normal), 1);
    return;
  #elif defined(TEXTURE_BUMP)
    fragment_color = vec4(encode_signed_normalized_vector_as_color(vec3(height)), 1);
    return;
  #elif defined(TEXTURE_SMOOTHENESS)
    fragment_color = vec4(vec3(srmo[0]), 1);
    return;
  #elif defined(TEXTURE_REFLECTIVITY)
    fragment_color = vec4(vec3(srmo[1]), 1);
    return;
  #elif defined(TEXTURE_METALLIC)
    fragment_color = vec4(vec3(srmo[2]), 1);
    return;
  #elif defined(TEXTURE_AO)
    fragment_color = vec4(vec3(srmo[3]), 1);
    return;
  #elif defined(TEXTURE_EMISSION)
    fragment_color = vec4(vec3(emission), 1);
    return;
  #endif
  
  float normal_length = length(normal);
  normal = normalize(tangent_to_worldspace * normal);
  
  srmo = mix(material_instance.srmo_range_0, material_instance.srmo_range_1, srmo);
  
  float smoothness = srmo[0];
  float reflectance = srmo[1];
  float metal_mask = srmo[2];
  float occlusion = srmo[3];
  
  emission *= material_instance.emission_factor;
  
  material.normal = normal;
  material.normal_length = normal_length;
  material.base_color = color.rgb;
  material.metal_mask = metal_mask;
  material.emission = emission;
  material.reflectance = reflectance;
  material.occlusion = occlusion;
  material.smoothness = smoothness;
  
  surface.position = fragment.position;
  
  #ifdef TWO_SIDED
  material.normal.z = mix(-material.normal.z, material.normal.z, gl_FrontFacing);
  #endif
#endif
  alpha = color.a;
}


void main()
{
#ifdef DEPTH_PREPASS
#if defined(OPAQUE) || defined(MATERIAL_NORMAL_WS) || defined(MATERIAL_ALPHA)
  return;
#endif
#endif

  calc_tangent_to_worldspace();

  BaseMaterial material;
  SurfaceData surface;
  float alpha;
  calculate_material_output(material, surface, alpha);
  
#if defined(TEXTURE_BASECOLOR) || defined(TEXTURE_BASECOLOR_ALPHA) || defined(TEXTURE_NORMAL_LS) || defined(TEXTURE_BUMP) || defined(TEXTURE_SMOOTHENESS) || defined(TEXTURE_REFLECTIVITY) || defined(TEXTURE_METALLIC) || defined(TEXTURE_AO) || defined(TEXTURE_EMISSION)
  return;
#endif
  
//#if (defined(DEPTH_PREPASS) || defined(GBUFFER_FILL_PASS)) && defined(MASKED)
  if(alpha < MASK_THRESHOLD)
    discard;
//#endif
#ifdef DEPTH_PREPASS
  return;
#endif
  
  apply_material(material, surface, tangent_to_worldspace, alpha);
}

#else

#error No valid main function found for the material.fs

#endif


void calc_tangent_to_worldspace()
{
  vec3 fragment_normal = normalize(fragment.normal);
  vec3 fragment_tangent = normalize(fragment.tangent);
  vec3 fragment_bitangent = normalize(fragment.bitangent);
  tangent_to_worldspace = mat3(fragment_tangent, fragment_bitangent, fragment_normal);
}
