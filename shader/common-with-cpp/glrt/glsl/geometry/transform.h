inline vec3 transform_point(in mat4 t, in vec3 point)
{
  vec4 vector = vec4(point, 1);

  vector = t * vector;

  return vector.xyz / vector.w;
}

inline vec3 transform_direction(in mat4 t, in vec3 relative)
{
  vec4 vector = vec4(relative, 0);

  vector = t * vector;

  return vector.xyz;
}

inline Ray transform_ray(in mat4 t, in Ray ray)
{
  Ray r;
  r.origin = transform_point(t, ray.origin);
  r.direction = transform_direction(t, ray.direction);
  return r;
}
