#include "input-block.fs.glsl"

#ifdef FORWARD_RENDERER
#include "forward-lighting.fs.glsl"
#endif

vec4 checkerboard()
{
  ivec2 tileId = ivec2(floor(gl_FragCoord.xy / 8)) % 2;
  
  float tileBright = 0.4;
  float tileDark = 0.2;
  float tile = mix(tileBright, tileDark, tileId.x == tileId.y);
  
  return vec4(vec3(tile), 1);
}