#define GLSL_DEBUGGING_TYPE_NONE ivec4(0)
#define GLSL_DEBUGGING_TYPE_BOOL(d) ivec4(0, d, 0, 0)
#define GLSL_DEBUGGING_TYPE_INT(d) ivec4(1, d, 0, 0)
#define GLSL_DEBUGGING_TYPE_FLOAT(d) ivec4(2, d, 0, 0)
#define GLSL_DEBUGGING_TYPE_MAT(rows, columns) ivec4(3, rows, columns, 0)
#define GLSL_DEBUGGING_TYPE_RAY ivec4(1024, 0, 0, 0)

#define GLSL_DEBUGGING_LENGTH 32
