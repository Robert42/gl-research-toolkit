inline vec3 transform_point(in mat4 t, in vec3 point)
{
  vec4 vector = vec4(point, 1);

  vector = t * vector;

  return vector.xyz / vector.w;
}

inline vec3 transform_point(in mat4x3 t, in vec3 point)
{
  vec4 vector4 = vec4(point, 1);

  vec3 vector3 = t * vector4;

  return vector3;
}

inline vec3 transform_direction(in mat4 t, in vec3 relative)
{
  vec4 vector = vec4(relative, 0);

  vector = t * vector;

  return normalize(vector.xyz);
}

inline vec3 transform_direction(in mat4x3 t, in vec3 relative)
{
  vec4 vector4 = vec4(relative, 0);

  vec3 vector3 = t * vector4;

  return normalize(vector3);
}

inline Ray transform_ray(in mat4 t, in Ray ray)
{
  Ray r;
  r.origin = transform_point(t, ray.origin);
  r.direction = transform_direction(t, ray.direction);
  return r;
}

inline Ray transform_ray(in mat4x3 t, in Ray ray)
{
  Ray r;
  r.origin = transform_point(t, ray.origin);
  r.direction = transform_direction(t, ray.direction);
  return r;
}

inline mat4 inverseOf4x3As4x4(in mat4x3 t)
{
  mat4 m;
  m[0] = vec4(t[0], 0);
  m[1] = vec4(t[1], 0);
  m[2] = vec4(t[2], 0);
  m[3] = vec4(t[3], 1);
  
  return inverse(m);
}
