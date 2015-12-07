#version 450 core

#include "common-debug-line-visualization.vs.glsl"

#include <geometry/transform.glsl>

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform CameraBlock
{
  vec3 lookAt;
  float fov;
  vec3 upVector;
  float aspect;
  vec3 position;
  float clipNear;
  float clipFar;
  mat4 inverseViewProjectionMatrix;
  mat4 inverseViewMatrix;
}camera;

void main()
{
  vec3 ws_position;

  if(vertex_parameter1 > 0.5f)
    ws_position = transform_point(camera.inverseViewProjectionMatrix, vertex_position);
  else
    ws_position = transform_point(camera.inverseViewMatrix, vertex_position);

  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
