struct BrdfData_WithLight
{
  float LdotH;
  float NdotL;
};

struct BrdfData_Generic
{
  float NdotV;
  vec3 H;
  float NdotH;
  float roughness;
};

struct BaseMaterial
{
  vec3 normal;
  float smoothness;
  vec3 baseColor;
  float metalMask;
  vec3 emission;
  float reflectance;
  float occlusion;
};

struct SurfaceData
{
  vec3 position;
  vec3 direction_to_camera;
  vec3 normal;
  float roughness;
  vec3 diffuse_color;
  float metalMask;
  vec3 f0;
  vec3 reflectionDir;
  vec3 dominantSpecularDir;
};

// listing 16
vec3 getSpecularDominantDirArea(vec3 N, vec3 R, float NdotV, float roughness)
{
    // Simple linear approximation
    float lerpFactor = (1 - roughness);

    return normalize(mix(N, R, lerpFactor));
}

// listing 26
float computeSpecOcclusion(float NdotV, float AO, float roughness)
{
    return saturate(pow(NdotV + AO, exp2(-16.0f * roughness - 1.0f)) - 1.0f + AO);
}

void precomputeData(in BaseMaterial material,
                    in vec3 surface_position,
                    in vec3 camera_position,
                    out SurfaceData surface_data,
                    out BrdfData_Generic brdf_data)
{
  vec3 V = normalize(camera_position-surface_position);
  vec3 N = material.normal;
  float smoothness = material.smoothness;
  vec3 baseColor = material.baseColor;
  float metalMask = material.metalMask;
  float reflectance = material.reflectance;
  float AO = material.occlusion;
  
  vec3 R                  = reflect(-V, N);
  
  vec3 H                  = normalize(V + L);
  float NdotV             = abs(dot(N, V)) + 1e-5f; // avoid artifact
  float NdotH             = saturate(dot(N, H));
  
  vec3 dominantSpecularDir = getSpecularDominantDirArea(N, R, NdotV, roughness);
  
  float roughness = sq(1.f-smoothness); // 3.2.1 & Figure 12
  vec3 f0 = mix(vec3(0.16f * sq(reflectance)), baseColor, metalMask); // 3.2.1 & Appendix D
  float f90 = saturate(50.0 * dot(f0, vec3(0.33))); // listing 27
  vec3 diffuseColor = mix(baseColor, vec3(0), metalMask); // Appendix D
  
  float diffuse_occlusion = AO;
  float specular_occlusion = computeSpecOcclusion(NdotV, AO, roughness);
  
  surface_data.position = surface_position;
  surface_data.direction_to_camera = V;
  surface_data.normal = N;
  surface_data.roughness =roughness;
  surface_data.diffuse_color = diffuseColor;
  surface_data.metalMask = metalMask;
  surface_data.f0 = f0;
  surface_data.reflectionDir = R;
  surface_data.dominantSpecularDir = dominantSpecularDir;
  
  brdf_data.H                 = H;
  brdf_data.NdotV             = NdotV;
  brdf_data.NdotH             = NdotH;
  brdf_data.roughness         = roughness;
}

BrdfData_WithLight init_brdf_data_with_light(in vec3 N, in vec3 L, in vec3 H)
{
  BrdfData_WithLight p;

  p.LdotH             = saturate(dot(L, H));
  p.NdotL             = saturate(dot(N, L));
  
  return p;
}
