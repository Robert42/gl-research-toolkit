vec3 heatvision_color_grey(float normalized_value)
{
  return vec3(normalized_value);
}

vec4 heatvision_linear(float normalized_value)
{
  normalized_value = clamp(normalized_value, 0, 1);
  vec3 heatvision_color = heatvision_color_grey(normalized_value);
  return vec4(heatvision_color, 1);
}

vec4 heatvision_log(uint32_t value, uint32_t maxValue)
{
  return heatvision_linear(log2(float(value)) / log2(float(maxValue)));
}

vec4 heatvision_linear(uint32_t value, uint32_t maxValue)
{
  return heatvision_linear(float(value) / float(maxValue));
}

vec4 heatvision(uint32_t value, uint32_t maxValue)
{
#ifdef LOG_HEATVISION
  return heatvision_log(value, maxValue);
#else
  return heatvision_linear(value, maxValue);
#endif
}