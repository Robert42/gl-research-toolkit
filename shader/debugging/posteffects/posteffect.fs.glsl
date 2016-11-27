#include <extensions/command_list.glsl>

#ifdef LOG_HEATVISION_DEBUG_POSTEFFECT
#define LOG_HEATVISION
#endif

#define highlightColor_DEFINED

#include <glrt/glsl/math-glsl.h>
#include <scene/uniforms.glsl>
#include <debugging/normal.glsl>
#include <write-fragment-color.glsl>
#include <write-fragment-depth.glsl>

#include <lighting/rendering-equation.glsl>

#define POSTEFFECT_VISUALIZATION


struct PosteffectVisualizationData
{
  float distancefield_offset;
  bool showWorldPos;
  bool showNormals;
  bool useLighting;
  bool useDirectionalLighting;
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


void rayMarch(in Ray ray, inout vec4 color, out vec3 world_pos, out vec3 world_normal);

const vec3 far_plane_world_pos = vec3(inf, inf, 0.9999999);
const vec3 near_plane_world_pos = vec3(inf, inf, -1);


/*
The following colors and directions are created with blender and imported with:
for light in bpy.context.user_preferences.system.solid_lights:
    light.direction
    light.diffuse_color

Returns:
Vector((0.675000011920929, 0.7250000238418579, 0.1369306445121765))
Color((0.5681918263435364, 0.6234841346740723, 0.6859579086303711))
Vector((-0.375, -0.7750000357627869, 0.5086748003959656))
Color((0.47173696756362915, 0.5366537570953369, 0.5999999046325684))
Vector((-0.6413142681121826, 0.6855428814888, -0.34459683299064636))
Color((1.0, 0.7710601687431335, 0.5074158310890198))
*/
vec3 light_up_mesh_directional(in vec3 camera_direction, vec3 normal)
{
  mat3 m = matrix3x3ForDirection(-camera_direction, vec3(0,0,1));
  m = inverse(m);

  normal = m * normal;


  return encode_signed_normalized_vector_as_color(normal);
}

void main()
{
  Ray ray;
  ray.origin = scene.camera_position;
  ray.direction = normalize(fragment.look_target - scene.camera_position);
  
  vec3 world_pos = near_plane_world_pos;
  vec3 world_normal = vec3(0);
  
  vec4 fragment_color = vec4(1, 0.5, 0, 1);
  
  rayMarch(ray, fragment_color,  world_pos, world_normal);
  
  if(!posteffect_param.showNumSteps)
  {
    if(posteffect_param.showWorldPos)
      fragment_color.rgb = world_pos;
    else if(posteffect_param.showNormals)
      fragment_color.rgb = encode_signed_normalized_vector_as_color(world_normal);
    else if(posteffect_param.useDirectionalLighting)
      fragment_color.rgb = light_up_mesh_directional(ray.direction, world_normal);
    else if(posteffect_param.useLighting)
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
  }
  
  if(any(isinf(world_pos)))
  {
    gl_FragDepth = FragCoord_z_toFragDepth(world_pos.z);
  }else
  {
    vec4 point = vec4(world_pos, 1);
    
    point = fragment.view_projection * point;
    point /= point.w;
    
    write_fragment_depth(point.z);
  }

  write_fragment_color(fragment_color);
}

vec4 heatvision(uint32_t value)
{
  uint32_t whiteLevel = posteffect_param.stepCountAsWhite;
  uint32_t blackLevel = posteffect_param.stepCountAsBlack;
  blackLevel = min(whiteLevel, blackLevel);
  value = max(value, blackLevel);
  
  return heatvision(value-blackLevel, whiteLevel-blackLevel);
}

vec3 highlightColor()
{
  float t = posteffect_param.totalTime;
  
  if(mod(t, 0.2f) <  0.1)
    return vec3(1, 0, 1);
  
  return vec3(step(mod(t, 0.4f),  0.2));
}
