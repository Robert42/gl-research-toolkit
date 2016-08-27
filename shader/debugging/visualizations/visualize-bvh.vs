#version 450 core

#include "common-debug-line-visualization.vs.glsl"

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform NodeBlock
{
  uint16_t node_id;
};



void main()
{
  vec3 c = vec3(1, 0.5, 0);
  
  Sphere* bvh_inner_spheres = bvh_inner_bounding_spheres();
  Sphere* leaf_spheres = distance_fields_bounding_spheres();
  uint16_t* bvh_inner_nodes = bvh_inner_nodes();
  
  // ==== SPECIAL CASE HANDLING ====
  bool invalid_index = node_id >= distance_fields_num()-1;
  
  if(invalid_index)
  {
    pass_attributes_to_fragment_shader(vec3(0), c);
    return;
  }
  
  // ==== finding out the current depth ====
  
  uint16_t current_id = node_id;
  uint16_t node_level = uint16_t(0);
  for(int i = int(node_id)*2-1; i>=0; i--)
  {
    if(bvh_inner_nodes[i] == current_id)
    {
      current_id = uint16_t(i/2);
      node_level++;
    }
  }
  
  bool in_interesting_range = node_level >= scene.bvh_debug_depth_begin && node_level < scene.bvh_debug_depth_end;
  
  if(!in_interesting_range)
  {
    pass_attributes_to_fragment_shader(vec3(0), c);
    return;
  }
  
  vec3 ws_position;
  
  bool is_sphere = vertex_color.x>0.f;
  bool is_connection_to_left = vertex_color.y>0.f;
  bool is_connection_to_right = vertex_color.z>0.f;
  bool is_connection = is_connection_to_left || is_connection_to_right;
  
  uint16_t index_of_child = bvh_inner_nodes[int(node_id)*2 + (is_connection_to_left?0:1)];
  
  if(is_sphere)
  {
    ws_position = vertex_position * bvh_inner_spheres[node_id].radius + bvh_inner_spheres[node_id].origin;
  }else if(is_connection && node_level+uint16_t(1)<scene.bvh_debug_depth_end)
  {
    c = vec3(0.5, 0.95, 0.05);
    Sphere parent_sphere = bvh_inner_spheres[node_id];
    Sphere child_sphere;
    
    if((index_of_child & uint16_t(0x8000)) != uint16_t(0))
      child_sphere = leaf_spheres[index_of_child & uint16_t(0x7fff)];
    else
      child_sphere = bvh_inner_spheres[index_of_child];
    
    ws_position = mix(parent_sphere.origin, child_sphere.origin, vertex_position.x);
  }else
    ws_position = vec3(0);

  pass_attributes_to_fragment_shader(ws_position, c);
}
