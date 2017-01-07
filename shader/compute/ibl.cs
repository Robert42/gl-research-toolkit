#version 450 core
#include <extensions/common.glsl>

#include <scene/uniforms.glsl>
#include <debugging/normal.glsl>

layout(local_size_x=GROUPS_SIZE_X, local_size_y=GROUPS_SIZE_Y, local_size_z=GROUPS_SIZE_Z) in;

struct IblHeader
{
  layout(rgba16f) writeonly image2D targetTexture;
  sampler2D sourceTexture;
  mat4 cube_side;
  float roughness;
  int sampleCount;
};

layout(binding=IBL_BLOCK, std140)
uniform IblHeaderBlock
{
  IblHeader header;
};

vec3 view_dir(ivec2 tex_coord);
vec3 environment_color(vec3 view);

#include <pbs/pbs.glsl>

void main()
{
  int sampleCount = header.sampleCount;
  ivec2 texture_coordinate = ivec2(gl_GlobalInvocationID.xy);
  
  vec3 v = view_dir(texture_coordinate);
  
  vec3 result;
  
  vec3 n = v;
#if GGX
  float roughness = header.roughness;
  v = vec3(0,0,1);
  result = integrateCubeLDOnly(v, n, roughness, sampleCount);
#elif DIFFUSE
  result = integrateDiffuseCube(n, sampleCount).rgb;
#else
  result = environment_color(v);
#endif
  
  imageStore(header.targetTexture, texture_coordinate, vec4(result, 1.0));
}

vec3 view_dir(ivec2 texture_coordinate)
{
  ivec2 target_size = imageSize(header.targetTexture);
  
  vec2 x = vec2(texture_coordinate);
  vec2 w = vec2(target_size);
  
  vec3 v;
  
  v.xz = (2*x - w + 1) / w;
  v.y = 1;
    
  v = normalize(v);
  
  v = (header.cube_side*vec4(v, 0)).xyz;
    
  return v;
}

vec3 environment_color(vec3 view_direction)
{
  mat3 m = mat3(0,-1, 0,
                1, 0, 0,
                0, 0, 1);
  vec2 uv = viewdir_to_uv_coord(m * view_direction);
  
  return texture2D(header.sourceTexture, uv).rgb;
}

vec4 sample_environment(vec3 view, float mipLevel)
{
  vec3 incoming = environment_color(view);
  
  incoming = clamp(incoming, vec3(0), vec3(5));
  
  return vec4(incoming, 1);
}

vec2 Hammersley(uint n, uint N)
{
  float g_2 = 0.f;
  float base = 0.5;

  for(uint k=1; k<N; k<<=1)
  {
    if((n & k) != 0)
      g_2 += base;
    base *= 0.5;
  }

  return vec2(g_2, double(n)/double(N));
}


vec2 getSample(uint i, uint sampleCount)
{
  return Hammersley(i, sampleCount);
}
