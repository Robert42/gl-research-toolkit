#version 450 core
#include <extensions/command_list.glsl>

#include <glrt/glsl/math.h>
#include <scene/uniforms.vs.glsl>

in layout(location=0) vec2 position;

out FragmentBlock
{
  flat mat4 view_projection;
  vec3 look_target;  
}fragment;

void main()
{
  gl_Position = vec4(position, 0, 1);
  
  fragment.view_projection = scene.view_projection;
  
  mat4 invMat = inverse(scene.view_projection);
  
  fragment.look_target = (invMat * vec4(position, 0, 1)).xyz;
}
