

vec3 transform_point(in mat4 t, in vec3 point)
{
  vec4 vector = vec4(point, 1);

  vector = t * vector;

  return vector.xyz / vector.w;
}

vec3 transform_direction(in mat4 t, in vec3 relative)
{
  vec4 vector = vec4(relative, 0);

  vector = t * vector;

  return vector.xyz;
}