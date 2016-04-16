#version 450 core
#include <extensions/command_list.glsl>

#include "implementation/common-implementation.vs.glsl"
#include "implementation/vertex-material.vs.glsl"

void main()
{
  transform_vertex();
  apply_material();
}
