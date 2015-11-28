#version 450 core

out vec4 color;

#include "./same-dir.glsl"
#include     "sub-dir/sub-dir.glsl"   
#include   "global/factor.glsl"   

void main()
{
  color = vec4((same_dir + sub_dir) * factor, 1);
}
