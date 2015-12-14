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
