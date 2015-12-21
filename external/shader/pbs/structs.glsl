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
  float smoothness;
  vec3 base_color;
  float metal_mask;
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
  float metal_mask;
  vec3 f0;
  float f90;
  vec3 R;
  float diffuse_occlusion;
  vec3 dominant_specular_dir;
  vec3 dominant_diffuse_dir;
  float specular_occlusion;
  vec3 emission;
};

// listing 16
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

// listing 26
float computeSpecOcclusion(float NdotV, float AO, float roughness)
{
    return saturate(pow(NdotV + AO, exp2(-16.0f * roughness - 1.0f)) - 1.0f + AO);
}

void precomputeData(in BaseMaterial material,
                    in vec3 surface_position,
                    in vec3 camera_position,
                    out BrdfData_Generic brdf_data,
                    out SurfaceData surface_data)
{
  vec3 V = normalize(camera_position-surface_position);
  vec3 N = material.normal;
  float smoothness = material.smoothness;
  vec3 base_color = material.base_color;
  float metal_mask = material.metal_mask;
  float reflectance = material.reflectance;
  vec3 emission = material.emission;
  float AO = material.occlusion;
  
  vec3 R                  = reflect(-V, N);
  
  float NdotV             = abs(dot(N, V)) + 1e-5f; // avoid artifact
  
  float roughness = sq(1.f-smoothness); // 3.2.1 & Figure 12
  vec3 f0 = mix(vec3(0.16f * sq(reflectance)), base_color, metal_mask); // 3.2.1 & Appendix D
  float f90 = saturate(50.0 * dot(f0, vec3(0.33))); // listing 27
  vec3 diffuseColor = mix(base_color, vec3(0), metal_mask); // Appendix D
  
  roughness = clamp(roughness, 0.0025, 1.0); // avoid numerical errors
  
  float diffuse_occlusion = AO;
  float specular_occlusion = computeSpecOcclusion(NdotV, AO, roughness);
  
  vec3 dominant_specular_dir = getSpecularDominantDirArea(N, R, NdotV, roughness);
  vec3 dominant_diffuse_dir  = getDiffuseDominantDir(N, V, NdotV, roughness);
  
  surface_data.position = surface_position;
  surface_data.direction_to_camera = V;
  surface_data.normal = N;
  surface_data.roughness =roughness;
  surface_data.diffuse_color = diffuseColor;
  surface_data.metal_mask = metal_mask;
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
