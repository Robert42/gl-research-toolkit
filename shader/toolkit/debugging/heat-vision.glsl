vec3 heatvision_color_grey(float normalized_value)
{
  return vec3(normalized_value);
}

vec3 heatvision_color(float normalized_value)
{
  vec3 c[8];
  
  c[0] = vec3(0);
  c[1] = vec3(0.014, 0.068, 0.242)*0.6; // #204a87
  c[2] = vec3(0.107, 0.036, 0.133)*1.25; // #5c3566
  c[3] = vec3(0.178, 0.08, 0.198); // #75507b
  c[4] = vec3(0.604, 0, 0); // #cc0000
  c[5] = vec3(0.913, 0.191, 0); // #f57900
  c[6] = vec3(0.973, 0.815, 0.001); // #fce94f
  c[7] = vec3(1); // #
  
  float value = clamp(normalized_value * 7, 0, 7);
  
  return mix(c[int(floor(value))],
             c[int(ceil(value))],
             fract(value));
  
  return vec3(normalized_value);
}

vec4 heatvision_linear(float normalized_value, float alpha=1)
{
  normalized_value = clamp(normalized_value, 0, 1);
  
  #ifdef HEATVISION_COLORS
  vec3 heatvision_color = heatvision_color(normalized_value);
  #else
  vec3 heatvision_color = heatvision_color_grey(normalized_value);
  #endif
  
  return vec4(heatvision_color, alpha);
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
