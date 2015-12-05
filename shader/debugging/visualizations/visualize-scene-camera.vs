#version 450 core

#include "common-debug-line-visualization.vs.glsl"

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform CameraBlock
{
  vec4 fov_and_aspect_and_clipNear_and_clipFar;
  vec3 lookAt;
  vec3 upVector;
  vec3 position;
  vec3 frustumEdgeDir;
}camera;

void main()
{
  float fov = camera.fov_and_aspect_and_clipNear_and_clipFar[0];
  float aspect = camera.fov_and_aspect_and_clipNear_and_clipFar[1];
  float clipNear = camera.fov_and_aspect_and_clipNear_and_clipFar[2];
  float clipFar = camera.fov_and_aspect_and_clipNear_and_clipFar[3];
  
  vec3 ws_position = vertex_position;

  if(vertex_parameter1 > 0.5f)
  {
    vec3 p = camera.frustumEdgeDir * mix(0, 1, vertex_position.z);
    ws_position = p;
  }

  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
