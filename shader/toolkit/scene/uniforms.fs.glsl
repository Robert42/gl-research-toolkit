#include <lighting/light-structs.glsl>

struct SceneLightData
{
  SphereAreaLight* sphere_arealights_address;
  RectAreaLight* rect_arealights_address;
  uint32_t num_sphere_area_lights;
  uint32_t num_rect_area_lights;
};

struct SceneFragmentData
{
  vec3 camera_position;
  SceneLightData lights;
};

layout(std140, binding=UNIFORM_BINDING_SCENE_FRAGMENT_BLOCK) uniform SceneFragmentBlock
{
  SceneFragmentData scene;
};