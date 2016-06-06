#ifndef SCENE_UNIFORMS_GLSL
#define SCENE_UNIFORMS_GLSL

#include <lighting/light-structs.glsl>
#include <voxels/voxel-structs.glsl>
#include <alignment.glsl>
#include <glrt/glsl/math-glsl.h>


#ifdef LOG_HEATVISION_DEBUG_COSTS
#define LOG_HEATVISION
#endif
#include <debugging/heat-vision.glsl>

struct SceneLightData
{
  uint64_t sphere_arealights_address;
  uint64_t rect_arealights_address;
  uint32_t num_sphere_area_lights;
  uint32_t num_rect_area_lights;
};

struct SceneVoxelHeader
{
  uint64_t distance_field_worldtovoxelMat_array_address;
  uint64_t distance_field_voxelcount_array_address;
  uint64_t distance_field_spacefactor_array_address;
  uint64_t distance_field_textures_array_address;
  uint64_t distance_field_boundingsphere_array_address;
  padding1(uint64_t, _padding1);
  uint32_t num_distance_fields;
  padding3(uint32_t, _padding2);
};

struct SceneData
{
  mat4 view_projection;
  vec3 camera_position;
  float totalTime;
  SceneLightData lights;
  SceneVoxelHeader voxelHeader;
  uint32_t costsHeatvisionBlackLevel;
  uint32_t costsHeatvisionWhiteLevel;
};

#include <glrt/glsl/layout-constants.h>

layout(binding=UNIFORM_BINDING_SCENE_BLOCK, std140) uniform SceneBlock
{
  SceneData scene;
};


void get_sphere_lights(out uint32_t num_sphere_lights, out SphereAreaLight* sphere_lights)
{
  num_sphere_lights = scene.lights.num_sphere_area_lights;
  sphere_lights = (SphereAreaLight*)scene.lights.sphere_arealights_address;
}

void get_rect_lights(out uint32_t num_rect_lights, out RectAreaLight* rect_lights)
{
  num_rect_lights = scene.lights.num_rect_area_lights;
  rect_lights = (RectAreaLight*)scene.lights.rect_arealights_address;
}

uint32_t distance_fields_num()
{
  return scene.voxelHeader.num_distance_fields;
}

mat4* distance_fields_worldToVoxelSpace()
{
  return (mat4*)scene.voxelHeader.distance_field_worldtovoxelMat_array_address;
}

ivec3* distance_fields_voxelCount()
{
  return (ivec3*)scene.voxelHeader.distance_field_voxelcount_array_address;
}

WorldVoxelUvwSpaceFactor* distance_fields_spaceFactor()
{
  return (WorldVoxelUvwSpaceFactor*)scene.voxelHeader.distance_field_spacefactor_array_address;
}

sampler3D* distance_fields_texture()
{
  return (sampler3D*)scene.voxelHeader.distance_field_textures_array_address;
}

Sphere* distance_fields_bounding_spheres()
{
  return (Sphere*)scene.voxelHeader.distance_field_boundingsphere_array_address;
}

#ifndef POSTEFFECT_VISUALIZATION
vec4 heatvision(uint32_t value)
{
  uint32_t blackLevel = scene.costsHeatvisionBlackLevel;
  uint32_t whiteLevel = scene.costsHeatvisionWhiteLevel;
  blackLevel = min(whiteLevel, blackLevel);
  value = max(value, blackLevel);
  
  return heatvision(value-blackLevel, whiteLevel-blackLevel);
}
#endif

#endif // SCENE_UNIFORMS_GLSL