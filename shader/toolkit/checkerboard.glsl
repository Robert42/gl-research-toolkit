#ifndef CHECKERBOARD_GLSL
#define CHECKERBOARD_GLSL

vec4 checkerboard(vec4 tileBright, vec4 tileDark, int tileSize=8)
{
  ivec2 tileId = ivec2(floor(gl_FragCoord.xy / tileSize)) % 2;
  
  return mix(tileBright, tileDark, tileId.x == tileId.y);
}

vec4 checkerboard(vec3 tileBright, vec3 tileDark, int tileSize=8)
{
  return checkerboard(vec4(tileBright, 1), vec4(tileDark, 1), tileSize);
}

vec4 checkerboard(float tileBright=0.4, float tileDark=0.2, int tileSize=8)
{
  return checkerboard(vec3(tileBright), vec3(tileDark), tileSize);
}

#endif // CHECKERBOARD_GLSL
