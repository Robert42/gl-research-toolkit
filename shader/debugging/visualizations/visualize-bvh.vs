#version 450 core

#include "common-debug-line-visualization.vs.glsl"

#include <lighting/light-structs.glsl>

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform NodeBlock
{
  uint16_t node_id;
};



void main()
{
  const vec3 c = vertex_color;
  
  if(node_id >= distance_fields_num()-1)
  {
    pass_attributes_to_fragment_shader(vec3(0), c);
    return;
  }
  
  Sphere* bvh_inner_spheres = bvh_inner_bounding_spheres();
  uint16_t* bvh_inner_nodes = bvh_inner_nodes();
  
  vec3 ws_position = vertex_position * bvh_inner_spheres[node_id].radius + bvh_inner_spheres[node_id].origin;

  pass_attributes_to_fragment_shader(ws_position, c);
}
