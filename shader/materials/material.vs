#version 450 core
#extension GL_NV_gpu_shader5 : require
#extension GL_NV_command_list : require

layout(commandBindableNV)uniform;

#include "implementation/common-implementation.vs.glsl"

void main()
{
  transform_vertex();
}
