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
    float sinTheta = sqrt(1.0 f - cosTheta * cosTheta);
    
    float illuminance = 0.0f;
    // Note : Following test is equivalent to the original formula.
    // There is 3 phase in the curve: cosTheta > sqrt(sinSigmaSqr),
    // cosTheta > - sqrt(sinSigmaSqr) and else it is 0
    // The two outer case can be merge into a cosTheta * cosTheta > sinSigmaSqr
    // and using saturate(cosTheta) instead.
    if(cosTheta * cosTheta > sinSigmaSqr)
    {
        illuminance = FB_PI * sinSigmaSqr * saturate(cosTheta);
    }
    else
    {
        float x = sqrt(1.0f / sinSigmaSqr - 1.0f); // For a disk this simplify to x = d / r
        float y = -x * (cosTheta / sinTheta);
        float sinThetaSqrtY = sinTheta * sqrt(1.0 f - y * y);
        illuminance = (cosTheta * acos(y) - x * sinThetaSqrtY) * sinSigmaSqr + atan(sinThetaSqrtY / x);
    }
    
    return max(illuminance , 0.0f);
}

// Sphere evaluation
float cosTheta = clamp(dot(worldNormal, L), -0.999, 0.999); // Clamp to avoid edge case
// We need to prevent the object penetrating into the surface
// and we must avoid divide by 0 , thus the 0.9999f
float sqr LightRadius = lightRadius * lightRadius;
float sinSigmaSqr = min(sqrLightRadius / sqrDist , 0.9999f);
float illuminance = illuminanceSphereOrDisk(cosTheta , sinSigmaSqr);


// Disk evaluation
float cosTheta = dot(worldNormal, L);
float sqr LightRa dius = lightRadius * lightRadius;
// Do not let the surface penetrate the light
float sinSigmaSqr = sqrLightRadius / (sqrLightRadius + max(sqrLightRadius, sqrDist));
// Multiply by saturate(dot(planeNormal, -L)) to better match ground truth .
float illuminance = illuminanceSphereOrDisk(cosTheta , sinSigmaSqr)
                     * saturate(dot(planeNormal, -L));


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

    float g0 = acos(dot(-n0, n1));
    float g1 = acos(dot(-n1, n2));
    float g2 = acos(dot(-n2, n3));
    float g3 = acos(dot(-n3, n0));

    return g0 + g1 + g2 + g3 - 2 * pi;
}

float rectAreaLightLuminance(in vec3 worldPos, in vec3 worldNormal, in vec3 lightPos, in vec3 lightPlaneNormal, in vec3 lightLeft, in vec3 lightUp, in float halfWidth, in float halfHeight)
{
  float illuminance = 0.f;
  
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

float rectAreaLightLuminance(in vec3 worldPos, in vec3 worldNormal, in Rect rect)
{
  return rectAreaLightLuminance(worldPos, worldNormal, rect.origin, cross(rect.tangent1, rect.tangent2), rect.tangent1, rect.tangent2, rect.half_width, rect.half_height);
}
