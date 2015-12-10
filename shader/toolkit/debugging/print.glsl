#include <glrt/glsl/layout-constants.h>
#include <glrt/glsl/math.h>
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
  return distance(gl_FragCoord.xy, debugging_buffer.fragment_coord) <= debugging_buffer.treshold;
}

void implement_print_chunk(in DebuggingOutputChunk chunk)
{
  if(is_fragment_to_debug() && debugging_buffer.numberChunks < GLSL_DEBUGGING_LENGTH)
  {
    debugging_buffer.chunks[debugging_buffer.numberChunks] = chunk;
    debugging_buffer.numberChunks++;
  }
}

void implement_print_value(in ivec4 type, in mat4 values)
{
  DebuggingOutputChunk chunk;
  chunk.type = type;
  chunk.floatValues = values;
  implement_print_chunk(chunk);
}

void implement_print_value(in ivec4 type, in ivec4 values)
{
  DebuggingOutputChunk chunk;
  chunk.type = type;
  chunk.integerValues = values;
  implement_print_chunk(chunk);
}

void PRINT_VALUE(in bool v)
{
  ivec4 value = ivec4(int(v), 0, 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_BOOL(1), value);
}

void PRINT_VALUE(in bvec2 v)
{
  ivec4 value = ivec4(ivec2(v), 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_BOOL(2), value);
}

void PRINT_VALUE(in bvec3 v)
{
  ivec4 value = ivec4(ivec3(v), 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_BOOL(3), value);
}

void PRINT_VALUE(in bvec4 v)
{
  ivec4 value = ivec4(v);
  implement_print_value(GLSL_DEBUGGING_TYPE_BOOL(4), value);
}

void PRINT_VALUE(in int v)
{
  ivec4 value = ivec4(int(v), 0, 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_INT(1), value);
}

void PRINT_VALUE(in ivec2 v)
{
  ivec4 value = ivec4(ivec2(v), 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_INT(2), value);
}

void PRINT_VALUE(in ivec3 v)
{
  ivec4 value = ivec4(ivec3(v), 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_INT(3), value);
}

void PRINT_VALUE(in ivec4 v)
{
  ivec4 value = ivec4(v);
  implement_print_value(GLSL_DEBUGGING_TYPE_INT(4), value);
}

void PRINT_VALUE(in float v)
{
  mat4 value;
  value[0] = vec4(int(v), 0, 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_FLOAT(1), value);
}

void PRINT_VALUE(in vec2 v)
{
  mat4 value;
  value[0] = vec4(vec2(v), 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_FLOAT(2), value);
}

void PRINT_VALUE(in vec3 v)
{
  mat4 value;
  value[0] = vec4(vec3(v), 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_FLOAT(3), value);
}

void PRINT_VALUE(in vec4 v)
{
  mat4 value;
  value[0] = vec4(v);
  implement_print_value(GLSL_DEBUGGING_TYPE_FLOAT(4), value);
}

void PRINT_VALUE(in mat4 value)
{
  implement_print_value(GLSL_DEBUGGING_TYPE_MAT(4,4), value);
}

void PRINT_VALUE(in Ray r)
{
  mat4 value;
  value[0] = vec4(r.origin, 1);
  value[1] = vec4(r.direction, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_RAY, value);
}
