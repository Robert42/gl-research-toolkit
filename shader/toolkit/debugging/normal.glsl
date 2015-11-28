vec3 encode_direction_as_color(vec3 direction)
{
  return normalize(direction) * 0.5 + vec3(0.5);
}

vec3 decode_direction_from_color(vec3 color)
{
  return color * 2 - vec3(1);
}
