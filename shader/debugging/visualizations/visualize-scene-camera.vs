#version 450 core

#include "common-debug-line-visualization.vs.glsl"

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
}camera;

void main()
{
  vec3 ws_position = vertex_position;

  if(vertex_parameter1 > 0.5f)
  {
    //vec3 p = camera.frustumEdgeDir * mix(0, 1, vertex_position.z);
    //ws_position = p;
    
    vec4 p = camera.inverseViewProjectionMatrix * vec4(vertex_position, 1);
    ws_position = p.xyz / p.w;
  }

  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
