#include <glrt/glsl/math.h>

vec3 F_Schlick(in vec3 f0, in float f90, in float u)
{
  return f0 + (f90 - f0) * pow(max(0.f, 1.f - u), 5.f);
}

float V_SmithGGXCorrelated(float NdotL, float NdotV, float alphaG)
{
  // Original formulation of G_SmithGGX Correlated
  // lambda_v = (-1 + sqrt(alphaG2 * (1 - NdotL2) / NdotL2 + 1)) * 0.5f;
  // lambda_l = (-1 + sqrt(alphaG2 * (1 - NdotV2) / NdotV2 + 1)) * 0.5f;
  // G_SmithGGXCorrelated = 1 / (1 + lambda_v + lambda_l) ;
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

vec3 getSpecularDominantDirArea(vec3 N, vec3 R, float NdotV, float roughness)
{
    // Simple linear approximation
    float lerpFactor = (1 - roughness);

    return normalize(mix(N, R, lerpFactor));
}

struct BrdfParameters
{
  float NdotV;
  float LdotH;
  float NdotH;
  float NdotL;
  float roughness;
};

BrdfParameters init_brdf_parameters(in vec3 N, in vec3 V, in vec3 L)
{
  BrdfParameters p;

  // This code is an example of call of previous functions
  p.NdotV             = abs(dot(N, V)) + 1e-5f; // avoid artifact
  vec3 H              = normalize(V + L);
  p.LdotH             = saturate(dot(L, H));
  p.NdotH             = saturate(dot(N, H));
  p.NdotL             = saturate(dot(N, L));
  
  return p;
}

// Specular BRDF
vec3 brdf_specular(in BrdfParameters param, float roughness, in vec3 f0, in float f90)
{
  float LdotH   = param.LdotH;
  float NdotV   = param.NdotV;
  float NdotL   = param.NdotL;
  float NdotH   = param.NdotH;
  
  vec3 F        = F_Schlick(f0 , f90, LdotH);
  float Vis     = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
  float D       = D_GGX (NdotH, roughness);
  vec3 Fr       = D * F * Vis / pi;
  
  return Fr;
}

// Diffuse BRDF
float brdf_diffuse(in BrdfParameters param, float linearRoughness)
{
  float LdotH   = param.LdotH;
  float NdotV   = param.NdotV;
  float NdotL   = param.NdotL;
  
  float Fd      = Fr_DisneyDiffuse(NdotV, NdotL, LdotH, linearRoughness) / pi;
  
  return Fd;
}


