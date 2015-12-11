#define GLSL_DEBUGGING_TYPE_NONE ivec3(0)
#define GLSL_DEBUGGING_TYPE_BOOL(d) ivec3(1, d, 0)
#define GLSL_DEBUGGING_TYPE_INT(d) ivec3(2, d, 0)
#define GLSL_DEBUGGING_TYPE_FLOAT(d) ivec3(3, d, 0)
#define GLSL_DEBUGGING_TYPE_MAT(rows, columns) ivec3(4, rows, columns)
#define GLSL_DEBUGGING_TYPE_RAY ivec3(1024, 0, 0)

#define GLSL_DEBUGGING_LENGTH 32
