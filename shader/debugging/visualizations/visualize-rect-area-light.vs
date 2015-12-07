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
  vec3 tangent = rectAreaLight.tangent;
  vec3 normal = rectAreaLight.normal;
  vec3 bitangent = cross(normal, tangent);
  
  vec3 ws_position = vertex_position.x*tangent*rectAreaLight.halfWidth +
                     vertex_position.y*bitangent*rectAreaLight.halfHeight +
                     vertex_position.z*normal*mix(rectAreaLight.halfWidth, rectAreaLight.halfHeight, 0.5) +
                     rectAreaLight.origin;

  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
