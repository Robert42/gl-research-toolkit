#ifndef ONLY_Fr_DisneyDiffuse
#include <glrt/glsl/math-glsl.h>
#endif

vec3 F_Schlick(in vec3 f0, in float f90, in float u)
{
  return f0 + (f90 - f0) * pow(max(0.f, 1.f - u), 5.f);
}

float V_SmithGGXCorrelated(float NdotL, float NdotV, float alphaG)
{
  // Original formulation of G_SmithGGX Correlated
  // lambda_v = (-1 + sqrt(alphaG2 * (1 - NdotL2) / NdotL2 + 1)) * 0.5f;
  // lambda_l = (-1 + sqrt(alphaG2 * (1 - NdotV2) / NdotV2 + 1)) * 0.5f;
  // G_SmithGGXCorrelated = 1 / (1 + lambda_v + lambda_l);
  // V_SmithGGXCorrelated = G_SmithGGXCorrelated / (4.0 f * NdotL * NdotV);
  
  // This is the optimize version
  float alphaG2 = alphaG * alphaG;
  // Caution : the " NdotL *" and " NdotV *" are explicitely inversed , this is not a mistake .
  float Lambda_GGXV = NdotL * sqrt((-NdotV * alphaG2 + NdotV) * NdotV + alphaG2);
  float Lambda_GGXL = NdotV * sqrt((-NdotL * alphaG2 + NdotL) * NdotL + alphaG2);
  
  return 0.5f / (Lambda_GGXV + Lambda_GGXL) ;
}

float D_GGX(float NdotH, float m)
{
  // Divide by PI is apply later
  float m2 = m * m ;
  float f = (NdotH * m2 - NdotH) * NdotH + 1;
  return m2 / (f * f);
}

float Fr_DisneyDiffuse(float NdotV, float NdotL, float LdotH,
                       float linearRoughness)
{
    float energyBias      = mix(0.f, 0.5f, linearRoughness);
    float energyFactor    = mix(1.0, 1.0/1.51, linearRoughness);
    float fd90            = energyBias + 2.0 * LdotH * LdotH * linearRoughness;
    vec3 f0               = vec3(1.0f, 1.0f, 1.0f);
    float lightScatter    = F_Schlick(f0, fd90, NdotL).r;
    float viewScatter     = F_Schlick(f0, fd90, NdotV).r;
    
    return lightScatter * viewScatter * energyFactor;
}

#ifndef ONLY_Fr_DisneyDiffuse

// Specular BRDF
vec3 brdf_specular(in BrdfData_Generic brdf_data_g, in BrdfData_WithLight brdf_data_l, in vec3 f0, in float f90)
{
  float LdotH     = brdf_data_l.LdotH;
  float NdotV     = brdf_data_g.NdotV;
  float NdotL     = brdf_data_l.NdotL;
  float NdotH     = brdf_data_l.NdotH;
  float roughness = brdf_data_g.roughness;
  
  vec3 F        = F_Schlick(f0 , f90, LdotH);
  float Vis     = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
  float D       = D_GGX (NdotH, roughness);
  vec3 Fr       = D * F * Vis / pi;
  
  return Fr;
}

// Diffuse BRDF
float brdf_diffuse(in BrdfData_Generic brdf_data_g, in BrdfData_WithLight brdf_data_l)
{
  float LdotH   = brdf_data_l.LdotH;
  float NdotV   = brdf_data_g.NdotV;
  float NdotL   = brdf_data_l.NdotL;
  float roughness = brdf_data_g.roughness;
  
  float Fd      = Fr_DisneyDiffuse(NdotV, NdotL, LdotH, roughness) / pi;
  
  return Fd;
}


#endif
