float Fr_DisneyDiffuse(float NdotV, float NdotL, float LdotH,
                       float linearRoughness)
{
    float energyBias      = lerp(0, 0.5, linearRoughness);
    float energyFactor    = lerp(1.0, 1.0/1.51, linearRoughness);
    float fd90            = energyBias + 2.0 * LdotH * LdotH * linearRoughness;
    float3 f0             = float3(1.0f, 1.0f, 1.0f);
    float lightScatter    = F_Schlick(f0, fd90, NdotL).r;
    float viewScatter     = F_Schlick(f0, fd90, NdotV).r;
    
    return lightScatter * viewScatter * energyFactor;
}
