#ifdef DEFERRED_RENDERER
layout(location=0) out vec4 worldNormal_normalLength;
layout(location=1) out vec4 meshNormal_meshBiTangentX;
layout(location=2) out vec4 meshTangent_meshBiTangentY;
layout(location=3) out float meshBiTangentZ;
layout(location=4) out vec4 baseColor_metalMask;
layout(location=5) out vec4 emission_reflectance;
layout(location=6) out vec2 occlusion_smoothness;

void write_fragment_color(vec4 fragment_color)
{
  worldNormal_normalLength = vec4(0,0,1,1);
  meshNormal_meshBiTangentX = vec4(0,0,1,0);
  meshTangent_meshBiTangentY = vec4(0,0,1,0);
  meshBiTangentZ = 0;
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
