#ifndef _GLRT_GLSL_MATH_H_
#define _GLRT_GLSL_MATH_H_

const float pi     = 3.1415926535897932384626433832795;
const float inv_pi = 0.3183098861837906715377675267450;
const float two_pi = 6.2831853071795864769252867665590;

const float inf = 1./0.;

inline float sq(float x)
{
  return x*x;
}

inline float sq(vec2 x)
{
  return dot(x, x);
}

inline float sq(vec3 x)
{
  return dot(x, x);
}

inline float sq(vec4 x)
{
  return dot(x, x);
}

inline float sq_distance(vec2 a, vec2 b)
{
  return sq(a-b);
}

inline float sq_distance(vec3 a, vec3 b)
{
  return sq(a-b);
}

inline float sq_distance(vec4 a, vec4 b)
{
  return sq(a-b);
}

// min

inline int min3(int a, int b, int c)
{
  return min(min(a, b), c);
}

inline int min4(int a, int b, int c, int d)
{
  return min(min(a, b), min(c, d));
}

inline float min3(float a, float b, float c)
{
  return min(min(a, b), c);
}

inline float min4(float a, float b, float c, float d)
{
  return min(min(a, b), min(c, d));
}

inline vec3 min3(vec3 a, vec3 b, vec3 c)
{
  return min(min(a, b), c);
}

inline vec3 min4(vec3 a, vec3 b, vec3 c, vec3 d)
{
  return min(min(a, b), min(c, d));
}

inline vec4 min3(vec4 a, vec4 b, vec4 c)
{
  return min(min(a, b), c);
}

inline vec4 min4(vec4 a, vec4 b, vec4 c, vec4 d)
{
  return min(min(a, b), min(c, d));
}

inline float min_component(vec2 vector)
{
  return min(vector.x, vector.y);
}

inline float min_component(vec3 vector)
{
  return min3(vector.x, vector.y, vector.z);
}

inline float min_component(vec4 vector)
{
  return min4(vector.x, vector.y, vector.z, vector.w);
}

inline int index_of_first_true(bvec3 boolean)
{
  boolean.yz = boolean.yz && not_(boolean.xx);
  boolean.z = boolean.z && !boolean.y;

  return int(dot(vec2(1,2), vec2(boolean.yz)));
}

inline int index_of_first_true(bvec4 boolean)
{
  boolean.yzw = boolean.yzw && not_(boolean.xxx);
  boolean.zw = boolean.zw && not_(boolean.yy);
  boolean.w = boolean.w && !boolean.z;

  return int(dot(vec3(1,2,3), vec3(boolean.yzw)));
}

inline bvec3 is_smallest(vec3 vector)
{
  return lessThanEqual(vector.xyz, min(vector.yzx, vector.zxy));
}

inline bvec4 is_smallest(vec4 vector)
{
  return lessThanEqual(vector.xyzw, min3(vector.yzwx, vector.zwxy, vector.wxyz));
}

inline bvec3 is_largest(vec3 vector)
{
  return greaterThanEqual(vector.xyz, min(vector.yzx, vector.zxy));
}

inline bvec4 is_largest(vec4 vector)
{
  return greaterThanEqual(vector.xyzw, min3(vector.yzwx, vector.zwxy, vector.wxyz));
}

// max

inline int max3(int a, int b, int c)
{
  return max(max(a, b), c);
}

inline int max4(int a, int b, int c, int d)
{
  return max(max(a, b), max(c, d));
}

inline float max3(float a, float b, float c)
{
  return max(max(a, b), c);
}

inline float max4(float a, float b, float c, float d)
{
  return max(max(a, b), max(c, d));
}

inline vec3 max3(vec3 a, vec3 b, vec3 c)
{
  return max(max(a, b), c);
}

inline vec3 max4(vec3 a, vec3 b, vec3 c, vec3 d)
{
  return max(max(a, b), max(c, d));
}

inline vec4 max3(vec4 a, vec4 b, vec4 c)
{
  return max(max(a, b), c);
}

inline vec4 max4(vec4 a, vec4 b, vec4 c, vec4 d)
{
  return max(max(a, b), max(c, d));
}

inline float max_component(vec2 vector)
{
  return max(vector.x, vector.y);
}

inline float max_component(vec3 vector)
{
  return max3(vector.x, vector.y, vector.z);
}

inline float max_component(vec4 vector)
{
  return max4(vector.x, vector.y, vector.z, vector.w);
}

inline vec3 _mask_out_for_(vec3 values, bvec3 mask)
{
  values = abs(values);
  return (values.xxx+values.yyy+values.zzz) * vec3(not_(mask));
}

inline vec4 _mask_out_for_(vec4 values, bvec4 mask)
{
  values = abs(values);
  return (values.xxxx+values.yyyy+values.zzzz+values.wwww) * vec4(not_(mask));
}

inline vec3 mask_out_for_min(vec3 values, bvec3 mask)
{
  // preventing  the components, where mask is false to be the smallest one
  return values + _mask_out_for_(values, mask);
}

inline vec3 mask_out_for_max(vec3 values, bvec3 mask)
{
  // preventing  the components, where mask is false to be the largest one
  return values - _mask_out_for_(values, mask);
}

inline vec4 mask_out_for_min(vec4 values, bvec4 mask)
{
  // preventing  the components, where mask is false to be the smallest one
  return values + _mask_out_for_(values, mask);
}

inline vec4 mask_out_for_max(vec4 values, bvec4 mask)
{
  // preventing  the components, where mask is false to be the largest one
  return values - _mask_out_for_(values, mask);
}


inline int index_of_min_component(vec3 vector)
{
  return index_of_first_true(is_smallest(vector));
}

inline int index_of_min_component_masked(vec3 vector, bvec3 mask)
{
  vector = mask_out_for_min(vector, mask);
  
  return index_of_first_true(is_smallest(vector));
}

inline int index_of_min_component(vec4 vector)
{
  return index_of_first_true(is_smallest(vector));
}

inline int index_of_min_component_masked(vec4 vector, bvec4 mask)
{
  vector = mask_out_for_min(vector, mask);
  
  return index_of_first_true(is_smallest(vector) && mask);
}


inline int index_of_max_component(vec3 vector)
{
  return index_of_first_true(is_largest(vector));
}

inline int index_of_max_component_masked(vec3 vector, bvec3 mask)
{
  vector = mask_out_for_min(vector, mask);

  return index_of_first_true(is_largest(vector));
}

inline int index_of_max_component(vec4 vector)
{
  return index_of_first_true(is_largest(vector));
}

inline int index_of_max_component_masked(vec4 vector, bvec4 mask)
{
  vector = mask_out_for_min(vector, mask);

  return index_of_first_true(is_largest(vector) && mask);
}


inline float min_component_masked(vec3 vector, bvec3 mask)
{
  return min_component(mask_out_for_min(vector, mask));
}

inline float min_component_masked(vec4 vector, bvec4 mask)
{
  return min_component(mask_out_for_min(vector, mask));
}

inline float max_component_masked(vec3 vector, bvec3 mask)
{
  return max_component(mask_out_for_max(vector, mask));
}

inline float max_component_masked(vec4 vector, bvec4 mask)
{
  return max_component(mask_out_for_max(vector, mask));
}

// blending

inline float blend_screen(float x,float y)
{
  return 1.f-(1.f-x)*(1.f-y);
}

inline vec2 blend_screen(vec2 x, vec2 y)
{
  return vec2(1)-(vec2(1)-x)*(vec2(1)-y);
}

inline vec3 blend_screen(vec3 x, vec3 y)
{
  return vec3(1)-(vec3(1)-x)*(vec3(1)-y);
}

inline vec4 blend_screen(vec4 x, vec4 y)
{
  return vec4(1)-(vec4(1)-x)*(vec4(1)-y);
}

// voxels

inline vec3 centerPointOfVoxel(in ivec3 voxelIndex)
{
  return vec3(voxelIndex) + 0.5f;
}

inline ivec3 voxelIndexFromScalarIndex(int index, ivec3 voxelCount)
{
  ivec3 voxelCoord;
  
  voxelCoord.x = index % voxelCount.x;
  voxelCoord.y = (index/voxelCount.x) % voxelCount.y;
  voxelCoord.z = index/(voxelCount.x*voxelCount.y);
  return voxelCoord;
}

inline mat3 matrix3x3ForDirection(vec3 direction, vec3 up)
{
  vec3 z = direction;
  vec3 x;
  vec3 y;

  x = cross(up, z);

  x = normalize(x);
  y = cross(x, z);

  return mat3(x, y, z);
}

inline mat3 matrix3x3ForDirection(vec3 direction)
{
  vec3 z = direction;
  vec3 x;
  vec3 y;

  vec3 up;

  if(abs(dot(z, vec3(0,1,0))) < abs(dot(z, vec3(0, 0, 1))))
    up = vec3(0, 1, 0);
  else
    up = vec3(0, 0, 1);

  return matrix3x3ForDirection(direction, up);
}

inline mat4 matrixForDirection(vec3 direction, vec3 origin=vec3(0))
{
  mat4 t = mat4(matrix3x3ForDirection(direction));
  t[0][3] = 0;
  t[1][3] = 0;
  t[2][3] = 0;
  t[3] = vec4(origin, 1);

  return t;
}


#include <glrt/glsl/geometry/raytracing.h>
#include <glrt/glsl/geometry/transform.h>


#endif // #define _GLRT_GLSL_MATH_H_
