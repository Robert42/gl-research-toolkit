#version 450 core

#include "common-debug-line-visualization.vs.glsl"

#include <lighting/light-structs.glsl>
#include <geometry/transform.glsl>

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform LightBlock
{
  SphereAreaLight sphereAreaLight;
};


void main()
{
  vec3 ws_position = vertex_position * sphereAreaLight.radius + sphereAreaLight.origin;

  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
