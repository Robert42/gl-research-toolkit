// Based on Listing 24

vec3 get_dfg_lut_value(float NdotV, float roughness);
vec3 get_environment_ibl_diffuse(vec3 view_direction);
vec3 get_environment_ibl_ggx(vec3 view_direction, float roughness);

vec3 evaluateIBLDiffuse(in vec3 V, in vec3 N, in vec3 R, in float roughness, float NdotV)
{
  vec3 dominantN = getDiffuseDominantDir(N, V, NdotV, roughness);
  vec3 diffuseLighting = get_environment_ibl_diffuse(dominantN);
  float diffF = get_dfg_lut_value(NdotV, roughness).z;
  return diffuseLighting * diffF;
}

#define DFG_TEXTURE_SIZE 256

vec3 evaluateIBLSpecular(vec3 f0, float f90, in vec3 N, in vec3 R, in float roughness, float NdotV)
{
  vec3 dominantR = getSpecularDominantDir(N, R, NdotV, roughness);
  // Rebuild the function
  // L . D . ( f0.Gv.(1-Fc) + Gv.Fc ) . cosTheta / (4 . NdotL . NdotV )
  NdotV = max(NdotV, 0.5f/DFG_TEXTURE_SIZE);
  vec3 preLD = get_environment_ibl_ggx(dominantR, roughness).rgb;
  // Sample pre - integrate DFG
  // Fc = (1-H.L)^5
  // PreIntegratedDFG.r = Gv.(1-Fc)
  // PreIntegratedDFG.g = Gv.Fc
  vec2 preDFG = get_dfg_lut_value(NdotV, roughness).xy;
  // LD.(f0.Gv.(1 - Fc) + Gv.Fc.f90)
  return preLD * (f0 * preDFG.x + f90 * preDFG.y);
}
