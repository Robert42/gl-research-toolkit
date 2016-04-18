// Listing 32

float adjustRoughness(float inputRoughness, float avgNormalLength)
{
  // Based on The Order : 1886 SIGGRAPH course notes implementation
  if(avgNormalLength < 1.0f)
  {
    float avgNormLen2 = avgNormalLength * avgNormalLength;
    float kappa = (3 * avgNormalLength - avgNormalLength * avgNormLen2) / (1 - avgNormLen2);
    float variance = 1.0f / (2.0 * kappa);
    return sqrt(inputRoughness * inputRoughness + variance);
  }
  return inputRoughness;
}