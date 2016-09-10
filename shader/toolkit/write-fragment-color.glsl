#ifdef DEFERRED_RENDERER
layout(location=0) out vec4 worldNormal_normalLength;
layout(location=1) out vec4 baseColor_metalMask;
layout(location=2) out vec4 emission_reflectance;
layout(location=3) out vec2 occlusion_smoothness;

void write_fragment_color(vec4 fragment_color)
{
  worldNormal_normalLength = vec4(0,0,1,1);
  baseColor_metalMask = vec4(0,0,0,0);
  emission_reflectance = vec4(fragment_color.rgb,0);
  occlusion_smoothness = vec2(0,0);
}
#endif

#ifdef FORWARD_RENDERER
out vec4 _color_out;

void write_fragment_color(vec4 fragment_color)
{
  _color_out = fragment_color;
}
#endif
