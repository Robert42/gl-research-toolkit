vec3 encode_signed_normalized_vector_as_color(vec3 direction)
{
  return direction * 0.5 + vec3(0.5);
}

vec3 encode_direction_as_color(vec3 direction)
{
  return encode_signed_normalized_vector_as_color(normalize(direction));
}

vec3 decode_direction_from_color(vec3 color)
{
  return color * 2 - vec3(1);
}
