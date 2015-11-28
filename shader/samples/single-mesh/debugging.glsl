const int DebuggingNone = 0;
const int DebuggingPlainColor = 1;
const int DebuggingNormals = 2;
const int DebuggingUV = 3;

uniform TestUniformBlock
{
  mat4 model_matrix;
  mat4 view_projection;
  vec4 material_color;
  vec3 light_direction;
  int debuggingMode;
} u;
