#include <extensions/command_list.glsl>

#include <glrt/glsl/math-glsl.h>
#include <scene/uniforms.glsl>
#include <debugging/normal.glsl>
#include <debugging/heat-vision.glsl>

#include <lighting/rendering-equation.glsl>


struct PosteffectVisualizationData
{
  float distancefield_offset;
  bool showNormals;
  bool useLighting;
  bool showNumSteps;
  uint32_t stepCountAsWhite;
  uint32_t stepCountAsBlack;
  float totalTime;
};

layout(binding=UNIFORM_BINDING_POSTEFFECTVISUALIZATION_BLOCK, std140) uniform PosteffectVisualizationDataBlock
{
  PosteffectVisualizationData posteffect_param;
};

in FragmentBlock
{
  flat mat4 view_projection;
  vec3 look_target;
}fragment;

float FragCoord_z_toFragDepth(float z)
{
  // TODO check for performance improvement by leaving out gl_DepthRange.diff and gl_DepthRange.near
  return (0.5 * z + 0.5) * gl_DepthRange.diff + gl_DepthRange.near;
}

void rayMarch(in Ray ray, inout vec4 color, out vec3 world_pos, out vec3 world_normal);

const vec3 far_plane_world_pos = vec3(inf, inf, 0.9999999);
const vec3 near_plane_world_pos = vec3(inf, inf, -1);

out vec4 fragment_color;
void main()
{
  Ray ray;
  ray.origin = scene.camera_position;
  ray.direction = normalize(fragment.look_target - scene.camera_position);
  
  vec3 world_pos = near_plane_world_pos;
  vec3 world_normal = vec3(0);
  
  fragment_color = vec4(1, 0.5, 0, 1);
  
  rayMarch(ray, fragment_color,  world_pos, world_normal);
  
  if(posteffect_param.showNormals && !posteffect_param.showNumSteps)
    fragment_color.rgb = encode_signed_normalized_vector_as_color(world_normal);
  if(posteffect_param.useLighting && !posteffect_param.showNumSteps)
  {
    BaseMaterial material;
    material.normal = world_normal;
    material.normal_length = 1.f;
    material.base_color = fragment_color.rgb;
    material.metal_mask = 0.f;
    material.emission = vec3(0.f);
    material.reflectance = 0.5f;
    material.occlusion = 1.f;
    material.smoothness = 0.5f;
  
    vec3 incoming_luminance = light_material(material, world_pos, scene.camera_position);
    fragment_color.rgb = accurateLinearToSRGB(incoming_luminance);
  }
  
  if(any(isinf(world_pos)))
  {
    gl_FragDepth = FragCoord_z_toFragDepth(world_pos.z);
  }else
  {
    vec4 point = vec4(world_pos, 1);
    
    point = fragment.view_projection * point;
    point /= point.w;
    
    gl_FragDepth = FragCoord_z_toFragDepth(point.z);
  }
}

vec4 heatvision(uint32_t value)
{
  uint32_t whiteLevel = posteffect_param.stepCountAsWhite;
  uint32_t blackLevel = posteffect_param.stepCountAsBlack;
  blackLevel = min(whiteLevel, blackLevel);
  value = max(value, blackLevel);
  
  return heatvision(value-blackLevel, whiteLevel-blackLevel);
}
