#version 450 core
#extension GL_NV_gpu_shader5 : require

#include "implementation/common-implementation.vs.glsl"

void main()
{
  transform_vertex();
}
