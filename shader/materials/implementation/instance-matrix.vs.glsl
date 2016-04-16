
#ifndef AREA_LIGHT

struct Instance
{
  mat4 model_matrix;
};

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140) uniform MeshInstanceBlock
{
  Instance instance;
};

mat4 get_model_matrix()
{
  return instance.model_matrix;
}

#else

mat4 get_model_matrix()
{
  return mat4(1);
}

#endif