#version 450 core
#include <extensions/common.glsl>

#include <glrt/glsl/math-glsl.h>
#include <random.glsl>




layout(local_size_x=GROUPS_SIZE_X, local_size_y=GROUPS_SIZE_Y, local_size_z=GROUPS_SIZE_Z) in;

struct VoxelizeMetaData
{
  int numVertices;
  int indexOffset;
  layout(r32f) writeonly image3D targetTexture;
  uint64_t vertices;
};

layout(binding=0, std140)
uniform VoxelizeMetaDataBlock
{
  VoxelizeMetaData metaData;
};

#ifndef STOCHASTIC_VOXELIZATION
#define STOCHASTIC_VOXELIZATION 1
#endif

#if !STOCHASTIC_VOXELIZATION
void main()
{
  int num_vertices = metaData.numVertices;
  vec3* vertices = vec3*(metaData.vertices);
  
  float best_positive_d = inf;
  float best_negative_d = -inf;
  float best_d_abs = inf;
  
  ivec3 textureSize = imageSize(metaData.targetTexture);
  
  const ivec3 voxelCoord = voxelIndexFromScalarIndex(int(gl_GlobalInvocationID.x)+metaData.indexOffset, textureSize);
  const vec3 p = centerPointOfVoxel(voxelCoord);

#ifdef MANIFOLD_RAY_CHECK
  Ray ray_x;
  Ray ray_y;
  Ray ray_z;
  
  ray_x.origin = p;
  ray_x.direction = vec3(1, 0, 0);
  ray_y.origin = p;
  ray_y.direction = vec3(0, 1, 0);
  ray_z.origin = p;
  ray_z.direction = vec3(0, 0, 1);

  int intersection_sign_sum = 0;
#endif

  for(int i=0; i<num_vertices; i+=3)
  {
    const vec3 v0 = vertices[i];
    const vec3 v1 = vertices[i+1];
    const vec3 v2 = vertices[i+2];

    vec3 uvw;
    vec3 closestPoint = closestPointOnTriangleToPoint(v0, v1, v2, p, uvw);

    float d_abs = distance(closestPoint, p);
    float d = -faceforward(vec3(d_abs,0,0), cross(v1-v0, v2-v0), p-closestPoint).x;
    
#ifdef MANIFOLD_RAY_CHECK
    intersection_sign_sum += int(triangle_ray_intersection_unclamped(ray_x, v0, v1, v2)) * int(sign(d));
    intersection_sign_sum += int(triangle_ray_intersection_unclamped(ray_y, v0, v1, v2)) * int(sign(d));
    intersection_sign_sum += int(triangle_ray_intersection_unclamped(ray_z, v0, v1, v2)) * int(sign(d));
#endif
    
    best_d_abs = min(d_abs, best_d_abs);
    
#if defined(MANIFOLD_RAY_CHECK) || defined(FACE_SIDE)
    best_positive_d = d >= 0 ? min(d, best_positive_d) : best_positive_d;
    best_negative_d = d <= 0 ? max(d, best_negative_d) : best_negative_d;
#endif
  }
  
  float best_d;
  
#if defined(TWO_SIDED)
  best_d = best_d_abs;
#elif defined(MANIFOLD_RAY_CHECK)
  if(intersection_sign_sum >= 0)
    best_d = best_positive_d;
  else
    best_d = best_negative_d;
#elif defined(FACE_SIDE)
  if(abs(best_negative_d) + 1.e-4f < best_positive_d)
    best_d = best_negative_d;
  else
    best_d = best_positive_d;
#else
#error Mode not recognized
#endif

  // Make sure, that voxels at the padding are never negative
  const int paddingWidth = 1;
  const bool isPadding = any(lessThan(voxelCoord, vec3(paddingWidth))) || any(greaterThanEqual(voxelCoord, vec3(textureSize-paddingWidth)));

  best_d = isPadding ? max(0, best_d) : best_d;
  
  if(all(lessThan(voxelCoord, textureSize)))
    imageStore(metaData.targetTexture, voxelCoord, vec4(best_d));
}

#elif STOCHASTIC_VOXELIZATION

void main()
{
  int num_vertices = metaData.numVertices;
  vec3* vertices = vec3*(metaData.vertices);

  ivec3 textureSize = imageSize(metaData.targetTexture);

  const ivec3 voxelCoord = voxelIndexFromScalarIndex(int(gl_GlobalInvocationID.x)+metaData.indexOffset, textureSize);
  const vec3 p = centerPointOfVoxel(voxelCoord);

  const uint num_rays = 10;

  int sign_heuristic = 0;

  float average_distance = 0.;


  for(uint i_ray=0; i_ray<num_rays; ++i_ray)
  {
    Ray ray;
    ray.origin = p;
    ray.direction = getSphereSample(i_ray, num_rays);

    float closest_distance = 65536;

    for(int i=0; i<num_vertices; i+=3)
    {
      const vec3 v0 = vertices[i];
      const vec3 v1 = vertices[i+1];
      const vec3 v2 = vertices[i+2];

      Plane triangle_plane = plane_from_three_points(v0, v1, v2);

      vec3 intersection_point;
      const bool has_intersection = triangle_ray_intersection_test(ray, v0, v1, v2, intersection_point);
      const float abs_distance = abs(dot(intersection_point - ray.origin, ray.direction));
      float distance_sign = sign(signed_distance_to(triangle_plane, p));
#if defined(TWO_SIDED)
      distance_sign = 1.f;
#endif
      const float signed_distance = abs_distance * distance_sign;

      closest_distance = mix(closest_distance, signed_distance, step(abs_distance, closest_distance) * float(has_intersection));
    }


    sign_heuristic += int(sign(closest_distance));
    average_distance += abs(closest_distance);
  }

  average_distance /= float(num_rays);

  float best_d = mix(-average_distance, average_distance, float(sign_heuristic<0));


  // Make sure, that voxels at the padding are never negative
  const int paddingWidth = 1;
  const bool isPadding = any(lessThan(voxelCoord, vec3(paddingWidth))) || any(greaterThanEqual(voxelCoord, vec3(textureSize-paddingWidth)));

  best_d = isPadding ? max(0, best_d) : best_d;

  if(all(lessThan(voxelCoord, textureSize)))
    imageStore(metaData.targetTexture, voxelCoord, vec4(best_d));
}

#endif
