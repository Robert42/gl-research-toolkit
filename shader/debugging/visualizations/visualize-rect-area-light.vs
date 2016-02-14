#version 450 core

#include "common-debug-line-visualization.vs.glsl"

#include <lighting/light-structs.glsl>
#include <geometry/transform.glsl>

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform LightBlock
{
  RectAreaLight rectAreaLight;
};


void main()
{
  vec3 tangent = rectAreaLight.tangent1;
  vec3 bitangent = rectAreaLight.tangent2;
  vec3 normal = cross(tangent, bitangent);
  
  vec3 ws_position = vertex_position.x*tangent*rectAreaLight.half_width +
                     vertex_position.y*bitangent*rectAreaLight.half_height +
                     vertex_position.z*normal*mix(rectAreaLight.half_width, rectAreaLight.half_height, 0.5) +
                     rectAreaLight.light.origin;

  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
