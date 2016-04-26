#include <lighting/light-structs.glsl>
#include <voxels/voxel-structs.glsl>
#include <alignment.glsl>

struct SceneLightData
{
  uint64_t sphere_arealights_address;
  uint64_t rect_arealights_address;
  uint32_t num_sphere_area_lights;
  uint32_t num_rect_area_lights;
};

struct SceneVoxelHeader
{
  uint64_t distance_field_aabbs_array_address;
  uint64_t distance_field_textures_array_address;
  uint32_t num_distance_fields;
  padding3(uint32_t, _padding);
};

struct SceneData
{
  mat4 view_projection;
  vec3 camera_position;
  SceneLightData lights;
  SceneVoxelHeader voxelHeader;
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

void get_distance_field_data(out uint32_t num_distance_fields, out VoxelData_AABB* distance_field_data_aabb)
{
  num_distance_fields = scene.voxelHeader.num_distance_fields;
  distance_field_data_aabb = (VoxelData_AABB*)scene.voxelHeader.distance_field_aabbs_array_address;
}
