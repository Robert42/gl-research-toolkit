
#ifndef AREA_LIGHT

struct Instance
{
  mat4 model_matrix;
};

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140) uniform MeshInstanceBlock
{
  Instance instance;
};

mat4 get_model_matrix()
{
  return instance.model_matrix;
}

#else // defined(AREA_LIGHT)

#if defined(SPHERE_LIGHT)
mat4 get_model_matrix()
{
  uint32_t n;
  SphereAreaLight* sphere_lights;
  get_sphere_lights(n, sphere_lights);
  
  vec3 translation = sphere_lights[gl_InstanceID].light.origin;
  float radius = sphere_lights[gl_InstanceID].radius;
  
  return mat4(radius, 0, 0, 0,
              0, radius, 0, 0,
              0, 0, radius, 0,
              translation, 1);
}
#elif defined(RECT_LIGHT)
mat4 get_model_matrix()
{
  uint32_t n;
  RectAreaLight* rect_lights;
  get_rect_lights(n, rect_lights);
  
  vec3 translation = rect_lights[gl_InstanceID].light.origin;
  vec3 tangent1 = rect_lights[gl_InstanceID].tangent1;
  float half_width = rect_lights[gl_InstanceID].half_width;
  vec3 tangent2 = rect_lights[gl_InstanceID].tangent2;
  float half_height = rect_lights[gl_InstanceID].half_height;
  vec3 normal = cross(tangent1, tangent2);
  
  return mat4(tangent1*half_width, 0,
              tangent2*half_height, 0,
              normal, 0,
              translation, 1);
}
#endif

#endif