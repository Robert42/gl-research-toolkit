#include <lighting/light-structs.glsl>

struct SceneLightData
{
  uint64_t sphere_arealights_address;
  uint64_t rect_arealights_address;
  uint32_t num_sphere_area_lights;
  uint32_t num_rect_area_lights;
};

struct SceneData
{
  mat4 view_projection;
  vec3 camera_position;
  SceneLightData lights;
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
