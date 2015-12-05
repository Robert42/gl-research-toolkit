#version 450 core

#include "common-debug-line-visualization.vs.glsl"

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform CameraBlock
{
  vec4 fov_and_aspect_and_clipNear_and_clipFar;
  vec3 lookAt;
  vec3 upVector;
  vec3 position;
}camera;

void main()
{
  float fov = camera.fov_and_aspect_and_clipNear_and_clipFar[0];
  float aspect = camera.fov_and_aspect_and_clipNear_and_clipFar[1];
  float clipNear = camera.fov_and_aspect_and_clipNear_and_clipFar[2];
  float clipFar = camera.fov_and_aspect_and_clipNear_and_clipFar[3];
  
  vec3 p = vertex_position;
  
  p = mix(vec3(0, 0, -clipNear), vec3(0, 0, -clipFar), p.z);
  
  p = mix(vertex_position, p, vertex_parameter1);
  
  pass_attributes_to_fragment_shader(p, vertex_color);
}
