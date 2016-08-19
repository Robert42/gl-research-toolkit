#ifndef ALIGNMENT_GLSL
#define ALIGNMENT_GLSL

#define padding1(type, name) type name##0
#define padding2(type, name) type name##0;type name##1
#define padding3(type, name) type name##0;type name##1;type name##2
#define padding4(type, name) type name##0;type name##1;type name##2;type name##3
#define padding5(type, name) type name##0;type name##1;type name##2;type name##3;type name##4
#define padding6(type, name) type name##0;type name##1;type name##2;type name##3;type name##4;type name##5
#define padding7(type, name) type name##0;type name##1;type name##2;type name##3;type name##4;type name##5;type name##6

#endif // ALIGNMENT_GLSL
