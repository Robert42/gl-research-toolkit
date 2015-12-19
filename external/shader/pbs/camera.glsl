// Listing 30
float3 approximationSRgbToLinear(in float3 sRGBCol)
{
  return pow(sRGBCol, 2.2);
}

float3 approximationLinearToSRGB(in float3 linearCol)
{
return pow(linearCol, 1 / 2.2);
}

float3 accurateSRGBToLinear(in float3 sRGBCol)
{
  float3 linearRGBLo = sRGBCol / 12.92;
  float3 linearRGBHi = pow((sRGBCol + 0.055) / 1.055, 2.4);
  float3 linearRGB = (sRGBCol <= 0.04045) ? linearRGBLo : linearRGBHi;
  return linearRGB ;
}

float3 accurateLinearToSRGB(in float3 linearCol)
{
  float3 sRGBLo = linearCol * 12.92;
  float3 sRGBHi = (pow(abs(linearCol), 1.0/2.4) * 1.055) - 0.055;
  float3 sRGB = (linearCol <= 0.0031308) ? sRGBLo : sRGBHi;
  return sRGB;
}

// Listing 28

float computeEV100 ( float aperture , float shutterTime , float ISO )
{
    // EV number is defined as:
    // 2^ EV_s = N^2 / t and EV_s = EV_100 + log2 (S /100)
    // This gives
    // EV_s = log2 (N^2 / t)
    // EV_100 + log2 (S /100) = log2 (N^2 / t)
    // EV_100 = log2 (N^2 / t) - log2 (S /100)
    // EV_100 = log2 (N^2 / t . 100 / S)
    return log2(sqr(aperture) / shutterTime * 100 / ISO);
}

float computeEV100FromAvgLuminance(float avgLuminance)
{
    // We later use the middle gray at 12.7% in order to have
    // a middle gray at 18% with a sqrt (2) room for specular highlights
    // But here we deal with the spot meter measuring the middle gray
    // which is fixed at 12.5 for matching standard camera
    // constructor settings (i.e. calibration constant K = 12.5)
    // Reference : http :// en. wikipedia . org / wiki / Film_speed
    return log2(avgLuminance * 100.0f / 12.5f);
}

float convertEV100ToExposure(float EV100)
{
  // Compute the maximum luminance possible with H_sbs sensitivity
  // maxLum = 78 / ( S * q ) * N^2 / t
  // = 78 / ( S * q ) * 2^ EV_100
  // = 78 / (100 * 0.65) * 2^ EV_100
  // = 1.2 * 2^ EV
  // Reference : http :// en. wikipedia . org / wiki / Film_speed
  float maxLuminance = 1.2f * pow(2.0f, EV100);
  return 1.0f / maxLuminance;
}

// usage with manual settings
float EV100 = computeEV100(aperture, shutterTime, ISO);
// usage with auto settings
float AutoEV100 = computeEV100FromAvgLuminance(Lavg);

float currentEV = useAutoExposure ? AutoEV100 : EV100 ;
float exposure = convertEV100toExposure(currentEV);

// exposure can then be used later in the shader to scale luminance
// if color is decomposed into XYZ
...
float exposedLuminance = luminance * exposure;
...
// or it can be applied directly on color
...
finalColor = color * exposure
...
