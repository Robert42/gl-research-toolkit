#ifndef ALIGNMENT_GLSL
#define ALIGNMENT_GLSL

#define padding1(type, name) type name##0
#define padding2(type, name) type name##0;type name##1
#define padding3(type, name) type name##0;type name##1;type name##2

#endif // ALIGNMENT_GLSL
