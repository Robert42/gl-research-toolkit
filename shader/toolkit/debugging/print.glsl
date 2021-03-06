
#include <glrt/glsl/layout-constants.h>
#include <glrt/glsl/geometry/structs.h>
#include <glrt/glsl/debugging/printer-types.h>

#ifdef COMPUTE_SHADER

#define PRINT_VALUE(x)
#define SHOW_VALUE(x)

#else

struct DebuggingOutputChunk
{
  ivec3 type;
  float z_value;
  mat4 floatValues;
  ivec4 integerValues;
};

#ifdef SHADER_DEBUG_PRINTER

struct DebuggingOutputData
{
  vec2 fragment_coord;
  float treshold;
  float offset;
  uint64_t chunkAddress;
};

layout(std140, binding=UNIFORM_BINDING_VALUE_PRINTER)
uniform DebuggingOutputBlock
{
  DebuggingOutputData debugging_buffer;
};
#endif

#ifdef SHADER_DEBUG_PRINTER
int g_suppress_print=0;
int nextChunkToWrite = 0;
#endif

void SUPPRESS_PRINT()
{
#ifdef SHADER_DEBUG_PRINTER
  g_suppress_print++;
#endif
}

void ALLOW_PRINT()
{
#ifdef SHADER_DEBUG_PRINTER
  g_suppress_print--;
#endif
}

bool is_fragment_to_debug()
{
#ifdef SHADER_DEBUG_PRINTER
  bool is_right_fragment = distance(gl_FragCoord.xy, debugging_buffer.fragment_coord+debugging_buffer.offset) <= debugging_buffer.treshold;
  bool is_suppressed = g_suppress_print>0;
  
  return is_right_fragment && !is_suppressed && nextChunkToWrite<GLSL_DEBUGGING_MAX_NUM_CHUNKS;
#else
  return false;
#endif
}

void implement_print_chunk(in DebuggingOutputChunk chunk)
{
#ifdef SHADER_DEBUG_PRINTER
  if(is_fragment_to_debug())
  {
    chunk.z_value = gl_FragCoord.z;
    
    DebuggingOutputChunk* chunks = (DebuggingOutputChunk*)debugging_buffer.chunkAddress;
    chunks[nextChunkToWrite] = chunk;
    nextChunkToWrite++; // chunks[nextChunkToWrite++] seems not to be supported
  }
#endif
}

void implement_print_value(in ivec3 type, in mat4 values, in bool visualize=false)
{
#ifdef SHADER_DEBUG_PRINTER
  DebuggingOutputChunk chunk;
  chunk.type = type;
  chunk.floatValues = values;
  chunk.integerValues = ivec4(float(visualize), 0, 0, 0);
  implement_print_chunk(chunk);
#endif
}

void implement_print_value(in ivec3 type, in ivec4 values)
{
#ifdef SHADER_DEBUG_PRINTER
  DebuggingOutputChunk chunk;
  chunk.type = type;
  chunk.integerValues = values;
  implement_print_chunk(chunk);
#endif
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

void PRINT_VALUE(in int32_t v)
{
  ivec4 value = ivec4(v, 0, 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_INT32(1), value);
}

void PRINT_VALUE(in uint32_t v)
{
  ivec4 value = ivec4(int(v), 0, 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_UINT32(1), value);
}

void PRINT_VALUE(in uint64_t v)
{
  uint32_t lower_bits = uint32_t(v&0xffffffff);
  uint32_t upper_bits = uint32_t(v >> 32);
  ivec4 value = ivec4(int(lower_bits), int(upper_bits), 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_UINT64(1), value);
}

void PRINT_VALUE(in ivec2 v)
{
  ivec4 value = ivec4(ivec2(v), 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_INT32(2), value);
}

void PRINT_VALUE(in ivec3 v)
{
  ivec4 value = ivec4(ivec3(v), 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_INT32(3), value);
}

void PRINT_VALUE(in ivec4 v)
{
  ivec4 value = ivec4(v);
  implement_print_value(GLSL_DEBUGGING_TYPE_INT32(4), value);
}

void PRINT_VALUE(in float v)
{
  mat4 value;
  value[0] = vec4(float(v), 0, 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_FLOAT(1), value);
}

void PRINT_VALUE(in vec2 v)
{
  mat4 value;
  value[0] = vec4(vec2(v), 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_FLOAT(2), value);
}

void PRINT_VALUE(in vec3 v, bool visualize=false)
{
  mat4 value;
  value[0] = vec4(vec3(v), 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_FLOAT(3), value, visualize);
}

void SHOW_VALUE(in vec3 v)
{
  PRINT_VALUE(v, true);
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

void PRINT_VALUE(in mat4x3 value)
{
  mat4 m = mat4(1);
  m[0].xyz = value[0];
  m[1].xyz = value[1];
  m[2].xyz = value[2];
  m[3].xyz = value[3];
  implement_print_value(GLSL_DEBUGGING_TYPE_MAT(4,3), m);
}

void PRINT_VALUE(in mat3 value)
{
  mat4 m = mat4(1);
  m[0].xyz = value[0];
  m[1].xyz = value[1];
  m[2].xyz = value[2];
  implement_print_value(GLSL_DEBUGGING_TYPE_MAT(3,3), m);
}

void PRINT_VALUE(in Sphere s)
{
  mat4 value;
  value[0] = vec4(s.origin, s.radius);
  implement_print_value(GLSL_DEBUGGING_TYPE_SPHERE, value);
}

void PRINT_VALUE(in Rect r)
{
  mat4 value;
  value[0] = vec4(r.origin, 1);
  value[1] = vec4(r.tangent1, r.half_width);
  value[2] = vec4(r.tangent2, r.half_height);
  implement_print_value(GLSL_DEBUGGING_TYPE_RECT, value);
}

void PRINT_VALUE(in Plane p)
{
  mat4 value;
  value[0] = vec4(p.normal, p.d);
  implement_print_value(GLSL_DEBUGGING_TYPE_PLANE, value);
}

void PRINT_VALUE(in Ray r, bool visualize=false)
{
  mat4 value;
  value[0] = vec4(r.origin, 1);
  value[1] = vec4(r.direction, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_RAY, value, visualize);
}

void SHOW_VALUE(in Ray r)
{
  PRINT_VALUE(r, true);
}

void PRINT_VALUE(in Cone cone, bool visualize=false)
{
  mat4 value;
  value[0] = vec4(cone.origin, 1);
  value[1] = vec4(cone.direction, 0);
  value[2] = vec4(cone.tan_half_angle, 0, 0, 0);
  implement_print_value(GLSL_DEBUGGING_TYPE_CONE, value, visualize);
}

void SHOW_VALUE(in Cone cone)
{
  PRINT_VALUE(cone, true);
}

#endif
