#version 450 core
#include <extensions/command_list.glsl>

#include <scene/uniforms.glsl>

in layout(location=0) vec2 position;

out FragmentBlock
{
  flat mat4 view_projection;
  vec3 look_target;  
}fragment;

void main()
{
  gl_Position = vec4(position.xy, 0, 1);
  
  fragment.view_projection = scene.view_projection;
  
  mat4 invMat = inverse(scene.view_projection);
  
  vec4 look_target_w = invMat * vec4(position, 1, 1);
  fragment.look_target = look_target_w.xyz / look_target_w.w;
}
