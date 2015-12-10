#include <glrt/glsl/layout-constants.h>
#include <glrt/glsl/debugging/printer-types.h>

layout(binding=UNIFORM_BINDING_VALUE_PRINTER_BLOCK, std140)
uniform DebuggingInputBlock
{
  vec2 fragment_coord;
  float treshold;
}debugging_input;

struct DebuggingOutputChunk
{
  ivec4 type;
  mat4 values2;
};

layout(binding=SHADERSTORAGE_BINDING_VALUE_PRINTER, std140)
buffer DebuggingOutputBlock
{
  int maxNumberChunks;
  int numberChunks;
  DebuggingOutputChunk chunks[];
}debugging_output;

bool is_fragment_to_debug()
{
  return distance(gl_FragCoord,debugging_input.fragment_coord) <= debugging_input.treshold;
}

void PRINT_VALUE(in vec3 value)
{
  // FIXME
}
