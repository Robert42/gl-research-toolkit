#version 450 core
#include <extensions/command_list.glsl>

#include <scene/uniforms.glsl>

out FragmentBlock
{
  flat mat4 view_projection;
  vec3 look_target;  
}fragment;

void main()
{
  vec2 position = vec2(gl_VertexID&1, gl_VertexID&2) * vec2(2.f, 1.f) - 1.f;

  gl_Position = vec4(position, 0, 1);
  
  fragment.view_projection = scene.view_projection;
  
  mat4 invMat = inverse(scene.view_projection);
  
  vec4 look_target_w = invMat * vec4(position, 1, 1);
  fragment.look_target = look_target_w.xyz / look_target_w.w;
}
