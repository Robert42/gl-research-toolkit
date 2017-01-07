struct BrdfData_WithLight
{
  vec3 H;
  float NdotH;
  float LdotH;
  float NdotL;
};

struct BrdfData_Generic
{
  float NdotV;
  float roughness;
};

struct BaseMaterial
{
  vec3 normal;
  float normal_length;
  vec3 base_color;
  float metal_mask;
  vec3 emission;
  float reflectance;
  float occlusion;
  float smoothness;
};

struct SurfaceData
{
  vec3 position;
  vec3 direction_to_camera;
  vec3 normal;
  float roughness;
  vec3 diffuse_color;
  vec3 f0;
  float f90;
  vec3 R;
  float diffuse_occlusion;
  vec3 dominant_specular_dir;
  vec3 dominant_diffuse_dir;
  float specular_occlusion;
  vec3 emission;
};

// listing 23
vec3 getDiffuseDominantDir(vec3 N, vec3 V, float NdotV, float roughness)
{
    float a = 1.02341f * roughness - 1.51174f;
    float b = -0.511705f * roughness + 0.755868f;
    float lerpFactor = saturate((NdotV * a + b) * roughness);

    return normalize(mix(N, V, lerpFactor));
}

// listing 16
vec3 getSpecularDominantDirArea(vec3 N, vec3 R, float NdotV, float roughness)
{
    // Simple linear approximation
    float lerpFactor = (1 - roughness);

    return normalize(mix(N, R, lerpFactor));
}

/*
// Based on Listing 22

// We have a better approximation of the off specular peak
// but due to the other ap proximat ions we found this one performs better .
// N is the normal direction
// R is the mirror vector
// This approximation works fine for G smith correlated and uncorrelated
float3 getSpecularDominantDir(float3 N, float3 R, float roughness)
{
  float smoothness = saturate(1 - roughness);
  float lerpFactor = smoothness * (sqrt(smoothness) + roughness);
  // The result is not normalized as we fetch in a cubemap
  return lerp(N, R, lerpFactor);
}
*/

// listing 26
float computeSpecOcclusion(float NdotV, float AO, float roughness)
{
    return saturate(pow(NdotV + AO, exp2(-16.0f * roughness - 1.0f)) - 1.0f + AO);
}

#define DEBUG_FRAGMENT_COLOR defined(MATERIAL_ROUGHNESS_UNADJUSTED) || defined(MATERIAL_ROUGHNESS_ADJUSTED)

#if DEBUG_FRAGMENT_COLOR
vec3 debug_fragment_color;
#endif

void precomputeData(in BaseMaterial material,
                    in vec3 surface_position,
                    in vec3 camera_position,
                    out BrdfData_Generic brdf_data,
                    out SurfaceData surface_data)
{
  vec3 V = normalize(camera_position-surface_position);
  vec3 N = material.normal;
  float normal_length = material.normal_length;
  vec3 base_color = material.base_color;
  float metal_mask = material.metal_mask;
  float reflectance = material.reflectance;
  vec3 emission = material.emission;
  float AO = material.occlusion;
  float smoothness = material.smoothness;
  
  #ifndef AREA_LIGHT
  
  #if defined(LIGHTING_ONLY)
  base_color = vec3(0.5);
  #endif
  
  #if defined(LIGHTING_SILVER) || defined(LIGHTING_MIRROR_PERFECT) || defined(LIGHTING_MIRROR_ROUGH) || defined(LIGHTING_MIRROR_SMOOTH)
  base_color = vec3(1);
  emission = vec3(0);
  metal_mask = 1;
  reflectance = 1;
  #if defined(LIGHTING_MIRROR_PERFECT)
  smoothness = 1;
  #elif defined(LIGHTING_MIRROR_ROUGH)
  smoothness = 0.5;
  #elif defined(LIGHTING_MIRROR_SMOOTH)
  smoothness = 0.8;
  #endif
  #endif
  
  #endif // AREA_LIGHT
  
  vec3 R                  = reflect(-V, N);
  
  float NdotV             = abs(dot(N, V)) + 1e-5f; // avoid artifact
  
  float roughness = sq(1.f-smoothness); // 3.2.1 & Figure 12
#ifdef MATERIAL_ROUGHNESS_UNADJUSTED
  debug_fragment_color = vec3(roughness);
#endif
#ifdef ADJUST_ROUGHNESS
  roughness = adjustRoughness(roughness, normal_length);
#endif
#ifdef MATERIAL_ROUGHNESS_ADJUSTED
  debug_fragment_color = vec3(roughness);
#endif
  vec3 f0 = mix(vec3(0.16f * sq(reflectance)), base_color, metal_mask); // 3.2.1 & Appendix D
  float f90 = saturate(50.0 * dot(f0, vec3(0.33))); // listing 27
  vec3 diffuseColor = mix(base_color, vec3(0), metal_mask); // Appendix D
  
  roughness = clamp(roughness, 0.001, 1.0); // avoid numerical errors
  
  float diffuse_occlusion = AO;
  float specular_occlusion = computeSpecOcclusion(NdotV, AO, roughness);
  
  vec3 dominant_specular_dir = getSpecularDominantDirArea(N, R, NdotV, roughness);
  vec3 dominant_specular_dir_ibl = getSpecularDominantDir(N, R, NdotV, roughness);
  vec3 dominant_diffuse_dir  = getDiffuseDominantDir(N, V, NdotV, roughness);
  
  surface_data.position = surface_position;
  surface_data.direction_to_camera = V;
  surface_data.normal = N;
  surface_data.roughness =roughness;
  surface_data.diffuse_color = diffuseColor;
  surface_data.f0 = f0;
  surface_data.f90 = f90;
  surface_data.R = R;
  surface_data.dominant_specular_dir = dominant_specular_dir;
  surface_data.dominant_diffuse_dir  = dominant_diffuse_dir;
  surface_data.diffuse_occlusion = diffuse_occlusion;
  surface_data.specular_occlusion = specular_occlusion;
  surface_data.emission = emission;
  
  brdf_data.NdotV             = NdotV;
  brdf_data.roughness         = roughness;
  
#ifdef LIGHTING_ONLY_SPECULAR
  surface_data.diffuse_color = vec3(0);
#endif
#ifdef LIGHTING_ONLY_DIFFUSE
  surface_data.f0 = vec3(0);
  surface_data.f90 = 0;
#endif
}

BrdfData_WithLight init_brdf_data_with_light(in vec3 N, in vec3 L, in vec3 V)
{
  BrdfData_WithLight p;
  
  vec3 H                  = normalize(V + L);
  
  p.H                     = H;
  p.NdotH                 = saturate(dot(N, H));
  p.LdotH                 = saturate(dot(L, H));
  p.NdotL                 = saturate(dot(N, L));
  
  return p;
}
