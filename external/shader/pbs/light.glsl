float rectangleSolidAngle(float3 worldPos,
                          float3 p0, float3 p1,
                          float3 p2, float3 p3)
{
    float3 v0 = p0 - worldPos;
    float3 v1 = p1 - worldPos;
    float3 v2 = p2 - worldPos;
    float3 v3 = p3 - worldPos;

    float3 n0 = normalize(cross(v0, v1));
    float3 n1 = normalize(cross(v1, v2));
    float3 n2 = normalize(cross(v2, v3));
    float3 n3 = normalize(cross(v3, v0));

    float g0 = acos(dot(-n0, n1));
    float g1 = acos(dot(-n1, n2));
    float g2 = acos(dot(-n2, n3));
    float g3 = acos(dot(-n3, n0));

    return g0 + g1 + g2 + g3 - 2 * FB_PI;
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
