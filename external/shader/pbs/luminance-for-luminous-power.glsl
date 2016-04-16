// Table 12

float luminance_for_sphere(float luminous_power, float radius)
{
  return luminous_power / (4.f * sq(radius * pi));
}

float luminance_for_disk(float luminous_power, float radius)
{
  return luminous_power / (sq(radius * pi));
}

float luminance_for_tube(float luminous_power, float radius, float width)
{
  return luminous_power / (2.f * pi * radius * width + 4.f * sq(radius * pi));
}

float luminance_for_rect(float luminous_power, float width, float height)
{
  return luminous_power / (width * height * pi);
}
