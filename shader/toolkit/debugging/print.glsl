#include <glrt/glsl/layout-constants.h>
#include <glrt/glsl/debugging/printer-types.h>

struct DebuggingOutputChunk
{
  ivec4 type;
  mat4 floatValues;
  ivec4 integerValues;
};

layout(binding=SHADERSTORAGE_BINDING_VALUE_PRINTER, std140)
buffer DebuggingOutputBlock
{
  vec2 fragment_coord;
  float treshold;
  int numberChunks;
  DebuggingOutputChunk chunks[];
}debugging_buffer;

bool is_fragment_to_debug()
{
  return distance(gl_FragCoord,debugging_input.fragment_coord) <= debugging_input.treshold;
}

void implement_print_value(in ivec4 type, in mat4 values)
{
  debugging_output.chunks[debugging_output.numberChunks].type = type;
  debugging_output.chunks[debugging_output.numberChunks].floatValues = values;
  debugging_output.numberChunks++;
}

void implement_print_value(in ivec4 type, in ivec4 values)
{
  debugging_output.chunks[debugging_output.numberChunks].type = type;
  debugging_output.chunks[debugging_output.numberChunks].integerValues = values;
  debugging_output.numberChunks++;
}

void PRINT_VALUE(in bool value)
{
  ivec4 value = ivec4(int(value), 0, 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_BOOL(1), value);
}

void PRINT_VALUE(in bvec2 value)
{
  ivec4 value = ivec4(ivec2(value), 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_BOOL(2), value);
}

void PRINT_VALUE(in bvec3 value)
{
  ivec4 value = ivec4(ivec3(value), 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_BOOL(3), value);
}

void PRINT_VALUE(in bvec4 value)
{
  ivec4 value = ivec4(value);
  implement_print_value(GLSL_DEBUGGING_TYPE_BOOL(4), value);
}

void PRINT_VALUE(in int value)
{
  ivec4 value = ivec4(int(value), 0, 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_INT(1), value);
}

void PRINT_VALUE(in ivec2 value)
{
  ivec4 value = ivec4(ivec2(value), 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_INT(2), value);
}

void PRINT_VALUE(in ivec3 value)
{
  ivec4 value = ivec4(ivec3(value), 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_INT(3), value);
}

void PRINT_VALUE(in ivec4 value)
{
  ivec4 value = ivec4(value);
  implement_print_value(GLSL_DEBUGGING_TYPE_INT(4), value);
}

void PRINT_VALUE(in float value)
{
  mat4 value;
  value[0] value = vec4(int(value), 0, 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_FLOAT(1), value);
}

void PRINT_VALUE(in vec2 value)
{
  mat4 value;
  value[0] value = vec4(ivec2(value), 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_FLOAT(2), value);
}

void PRINT_VALUE(in vec3 value)
{
  mat4 value;
  value[0] value = vec4(ivec3(value), 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_FLOAT(3), value);
}

void PRINT_VALUE(in vec4 value)
{
  mat4 value;
  value[0] value = vec4(value);
  implement_print_value(GLSL_DEBUGGING_TYPE_FLOAT(4), value);
}

void PRINT_VALUE(in mat4 value)
{
  implement_print_value(GLSL_DEBUGGING_TYPE_MAT(4,4), value);
}
