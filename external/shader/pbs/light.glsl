if(dot(worldPos - lightPos, lightPlaneNormal) > 0)
{
    float halfWidth  = lightWidth * 0.5;
    float halfHeight = lightHeight * 0.5;
    float3 p0 = lightPos + lightLeft * -halfWidth + lightUp *  halfHeight;
    float3 p1 = lightPos + lightLeft * -halfWidth + lightUp * -halfHeight;
    float3 p2 = lightPos + lightLeft *  halfWidth + lightUp * -halfHeight;
    float3 p3 = lightPos + lightLeft *  halfWidth + lightUp *  halfHeight;
    float solidAngle = rectangleSolidAngle(worldPos, p0, p1, p2, p3);

    float illuminance = solidAngle * 0.2 * (
                        saturate(dot(normalize(p0 - worldPos), worldNormal)) +
                        saturate(dot(normalize(p1 - worldPos), worldNormal)) +
                        saturate(dot(normalize(p2 - worldPos), worldNormal)) +
                        saturate(dot(normalize(p3 - worldPos), worldNormal)) +
                        saturate(dot(normalize(lightPos - worldPos), worldNormal)));
}