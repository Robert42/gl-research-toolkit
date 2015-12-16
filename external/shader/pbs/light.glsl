// ======== Disk-Area-Light ====================================================

// A right disk is a disk oriented to always face the lit surface.
// Solid angle of a sphere or a right disk is 2 PI (1 - cos(subtended angle)).
// Subtended angle sigma = arcsin(r / d) for a sphere
// and sigma = atan(r / d) for a right disk
// sinSigmaSqr = sin(subtended angle)^2 , it is (r ^2 / d ^2) for a sphere
// and (r ^2 / (r ^2 + d ^2)) for a disk
// cosTheta is not clamped
float illuminanceSphereOrDisk(float cosTheta, float sinSigmaSqr)
{
    float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
    
    float illuminance = 0.0f;
    // Note : Following test is equivalent to the original formula.
    // There is 3 phase in the curve: cosTheta > sqrt(sinSigmaSqr),
    // cosTheta > - sqrt(sinSigmaSqr) and else it is 0
    // The two outer case can be merge into a cosTheta * cosTheta > sinSigmaSqr
    // and using saturate(cosTheta) instead.
    if(cosTheta * cosTheta > sinSigmaSqr)
    {
        illuminance = pi * sinSigmaSqr * saturate(cosTheta);
    }
    else
    {
        float x = sqrt(1.0f / sinSigmaSqr - 1.0f); // For a disk this simplify to x = d / r
        float y = -x * (cosTheta / sinTheta);
        float sinThetaSqrtY = sinTheta * sqrt(1.0f - y * y);
        illuminance = (cosTheta * acos(y) - x * sinThetaSqrtY) * sinSigmaSqr + atan(sinThetaSqrtY / x);
    }
    
    return max(illuminance , 0.0f);
}

// Sphere evaluation
float sphereLightIlluminance(in vec3 worldNormal, in vec3 L, float sqrDist, float lightRadius)
{
    float cosTheta = clamp(dot(worldNormal, L), -0.999, 0.999); // Clamp to avoid edge case
    // We need to prevent the object penetrating into the surface
    // and we must avoid divide by 0 , thus the 0.9999f
    float sqrLightRadius = lightRadius * lightRadius;
    float sinSigmaSqr = min(sqrLightRadius / sqrDist , 0.9999f);
    float illuminance = illuminanceSphereOrDisk(cosTheta , sinSigmaSqr);
    
    return illuminance;
}


// Disk evaluation
float diskLightIlluminance(in vec3 worldNormal, in vec3 L, float sqrDist, in vec3 planeNormal, float lightRadius)
{
    float cosTheta = dot(worldNormal, L);
    float sqrLightRadius = lightRadius * lightRadius;
    // Do not let the surface penetrate the light
    float sinSigmaSqr = sqrLightRadius / (sqrLightRadius + max(sqrLightRadius, sqrDist));
    // Multiply by saturate(dot(planeNormal, -L)) to better match ground truth .
    float illuminance = illuminanceSphereOrDisk(cosTheta , sinSigmaSqr)
                         * saturate(dot(planeNormal, -L));
                         
    return illuminance;
}


float sphereLightIlluminance(in vec3 worldNormal, in vec3 worldPos, in Sphere sphere)
{
  vec3 unnormalizedL = sphere.origin - worldPos;
  vec3 L = normalize(unnormalizedL);
  float sqrDist = sq(unnormalizedL);
  
  return sphereLightIlluminance(worldNormal, L, sqrDist, sphere.radius);
}

float diskLightIlluminance(in vec3 worldNormal, in vec3 worldPos, in Disk disk)
{
  vec3 unnormalizedL = disk.origin - worldPos;
  vec3 L = normalize(unnormalizedL);
  float sqrDist = sq(unnormalizedL);
  
  return diskLightIlluminance(worldNormal, L, sqrDist, disk.normal, disk.radius);
}

vec3 getDirectionToLight(in Sphere sphere, vec3 worldPosition) // using the center of the light as approximnation (see 4.7.4 for alternatives)
{
  return normalize(sphere.origin-worldPosition);
}

vec3 getDirectionToLight(in Disk disk, vec3 worldPosition) // using the center of the light as approximnation (see 4.7.4 for alternatives)
{
  return normalize(disk.origin-worldPosition);
}


// ======== Rect-Area-Light ====================================================

float rectangleSolidAngle(vec3 worldPos,
                          vec3 p0, vec3 p1,
                          vec3 p2, vec3 p3)
{
    vec3 v0 = p0 - worldPos;
    vec3 v1 = p1 - worldPos;
    vec3 v2 = p2 - worldPos;
    vec3 v3 = p3 - worldPos;

    vec3 n0 = normalize(cross(v0, v1));
    vec3 n1 = normalize(cross(v1, v2));
    vec3 n2 = normalize(cross(v2, v3));
    vec3 n3 = normalize(cross(v3, v0));

    float g0 = clamp(acos(dot(-n0, n1)), -1, 1);
    float g1 = clamp(acos(dot(-n1, n2)), -1, 1);
    float g2 = clamp(acos(dot(-n2, n3)), -1, 1);
    float g3 = clamp(acos(dot(-n3, n0)), -1, 1);

    return g0 + g1 + g2 + g3 - 2 * pi;
}

float rectAreaLightIlluminance(in vec3 worldPos, in vec3 worldNormal, in vec3 lightPos, in vec3 lightPlaneNormal, in vec3 lightLeft, in vec3 lightUp, in float halfWidth, in float halfHeight)
{
  float illuminance = 0.f;
  
  // TODO this can be removed by multiplying with the boolean value of the condition. Measure performance instead of guessing
  if(dot(worldPos - lightPos, lightPlaneNormal) > 0)
  {
      vec3 p0 = lightPos + lightLeft * -halfWidth + lightUp *  halfHeight;
      vec3 p1 = lightPos + lightLeft * -halfWidth + lightUp * -halfHeight;
      vec3 p2 = lightPos + lightLeft *  halfWidth + lightUp * -halfHeight;
      vec3 p3 = lightPos + lightLeft *  halfWidth + lightUp *  halfHeight;
      float solidAngle = rectangleSolidAngle(worldPos, p0, p1, p2, p3);

      illuminance = solidAngle * 0.2 * (
                    saturate(dot(normalize(p0 - worldPos), worldNormal)) +
                    saturate(dot(normalize(p1 - worldPos), worldNormal)) +
                    saturate(dot(normalize(p2 - worldPos), worldNormal)) +
                    saturate(dot(normalize(p3 - worldPos), worldNormal)) +
                    saturate(dot(normalize(lightPos - worldPos), worldNormal)));
  }
  
  return illuminance;
}

float rectAreaLightIlluminance(in vec3 worldPos, in vec3 worldNormal, in Rect rect)
{
  return rectAreaLightIlluminance(worldPos, worldNormal, rect.origin, cross(rect.tangent1, rect.tangent2), rect.tangent1, rect.tangent2, rect.half_width, rect.half_height);
}

vec3 getDirectionToLight(in Rect rect, vec3 worldPosition) // using the center of the light as approximnation (see 4.7.4 for alternatives)
{
  return normalize(rect.origin-worldPosition);
}


// ======== Tube-Area-Light ====================================================

// Return the closest point on the line ( without limit )
vec3 closestPointOnLine(vec3 a, vec3 b, vec3 c)
{
    vec3 ab = b - a;
    float t = dot(c - a, ab) / dot(ab, ab);
    return a + t * ab;
}

// Return the closest point on the segment ( with limit )
vec3 closestPointOnSegment(vec3 a, vec3 b, vec3 c)
{
    vec3 ab = b - a;
    float t = dot(c - a , ab) / dot(ab, ab);
    return a + saturate(t) * ab;
}

float tubeAreaLightIlluminance(in vec3 worldPos, in vec3 worldNormal, in vec3 P0, in vec3 P1, in vec3 lightPos, in vec3 lightLeft, float lightWidth, float lightRadius)
{
    // The sphere is placed at the nearest point on the segment .
    // The rectangular plane is define by the following orthonormal frame :
    vec3 forward = normalize(closestPointOnLine(P0, P1, worldPos) - worldPos);
    vec3 left = lightLeft;
    vec3 up = cross(lightLeft , forward);

    vec3 p0 = lightPos - left * (0.5 * lightWidth) + lightRadius * up;
    vec3 p1 = lightPos - left * (0.5 * lightWidth) - lightRadius * up;
    vec3 p2 = lightPos + left * (0.5 * lightWidth) - lightRadius * up;
    vec3 p3 = lightPos + left * (0.5 * lightWidth) + lightRadius * up;

    float solidAngle = rectangleSolidAngle(worldPos, p0, p1, p2, p3);

    float illuminance = solidAngle * 0.2 * (
            saturate(dot(normalize(p0 - worldPos), worldNormal)) +
            saturate(dot(normalize(p1 - worldPos), worldNormal)) +
            saturate(dot(normalize(p2 - worldPos), worldNormal)) +
            saturate(dot(normalize(p3 - worldPos), worldNormal)) +
            saturate(dot(normalize(lightPos - worldPos), worldNormal)));

    // We then add the contribution of the sphere
    vec3 spherePosition = closestPointOnSegment(P0, P1, worldPos);
    vec3 sphereUnormL = spherePosition - worldPos;
    vec3 sphereL = normalize(sphereUnormL);
    float sqrSphereDistance = dot(sphereUnormL, sphereUnormL);

    float illuminanceSphere = pi * saturate(dot(sphereL, worldNormal)) *
                              ((lightRadius * lightRadius) / sqrSphereDistance);

    illuminance += illuminanceSphere;
    
    return illuminance;
}

float tubeAreaLightIlluminance(in vec3 worldPos, in vec3 worldNormal, in Tube tube)
{
  vec3 tubeOriginToP0 = tube.direction*tube.length*0.5f;
  
  return tubeAreaLightIlluminance(worldPos, worldNormal, tube.origin+tubeOriginToP0, tube.origin-tubeOriginToP0, tube.origin, tube.direction, tube.length, tube.radius);
}

vec3 getDirectionToLight(in Tube tube, vec3 worldPosition) // using the center of the light as approximnation (see 4.7.4 for alternatives)
{
  return normalize(tube.origin-worldPosition);
}
