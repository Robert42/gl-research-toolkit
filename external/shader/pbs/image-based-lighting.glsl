/*


// Based on Listing 24

float3 evaluateIBLDiffuse(...)
{
  float3 dominantN = getDiffuseDominantDir(N, V, NdotV, roughness);
  float3 diffuseLighting = diffuseLD.Sample(sampler, dominantN);
  float diffF = DFG.SampleLevel(sampler, float2(NdotV, roughness), 0).z;
  return diffuseLighting * diffF;
}

float3 evaluateIBLSpecular(...)
{
  float3 dominantR = getSpecularDominantDir(N, R, NdotV, roughness);
  // Rebuild the function
  // L . D . ( f0.Gv.(1-Fc) + Gv.Fc ) . cosTheta / (4 . NdotL . NdotV )
  NdotV = max(NdotV, 0.5f/DFG_TEXTURE_SIZE);
  float mipLevel = linearRoughnessToMipLevel(linearRoughness, mipCount);
  float3 preLD = specularLD.SampleLevel(sampler, dominantR, mipLevel).rgb;
  // Sample pre - integrate DFG
  // Fc = (1-H.L)^5
  // PreIntegratedDFG.r = Gv.(1-Fc)
  // PreIntegratedDFG.g = Gv.Fc
  float2 preDFG = DFG.SampleLevel(sampler, float2(NdotV, roughness), 0).xy;
  // LD.(f0.Gv.(1 - Fc) + Gv.Fc.f90)
  return preLD * (f0 * preDFG.x + f90 * preDFG.y);
}

*/
