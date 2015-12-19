vec3 chooseValue(in vec3 x, in vec3 treshold, in vec3 lower, in vec3 larger)
{
  bvec3 useLower = lessThanEqual(x, treshold);
  bvec3 useLarger = not(useLower);
  
  return float(useLower) * lower + float(useLarger) * larger;
}

// based on Listing 30

vec3 approximationSRgbToLinear(in vec3 sRGBCol)
{
  return pow(sRGBCol, vec3(2.2));
}

vec3 approximationLinearToSRGB(in vec3 linearCol)
{
  return pow(linearCol, vec3(1 / 2.2));
}

vec3 accurateSRGBToLinear(in vec3 sRGBCol)
{
  vec3 linearRGBLo = sRGBCol / 12.92;
  vec3 linearRGBHi = pow((sRGBCol + 0.055) / 1.055, vec3(2.4));
  
  return chooseValue(sRGBCol, vec3(0.04045), linearRGBLo, linearRGBHi);
}

vec3 accurateLinearToSRGB(in vec3 linearCol)
{
  vec3 sRGBLo = linearCol * 12.92;
  vec3 sRGBHi = (pow(abs(linearCol), vec3(1.0/2.4)) * 1.055) - 0.055;
  vec3 sRGB = chooseValue(linearCol, vec3(0.0031308),  sRGBLo, sRGBHi);
  return sRGB;
}

// based on Listing 28

float computeEV100 ( float aperture , float shutterTime , float ISO )
{
    // EV number is defined as:
    // 2^ EV_s = N^2 / t and EV_s = EV_100 + log2 (S /100)
    // This gives
    // EV_s = log2 (N^2 / t)
    // EV_100 + log2 (S /100) = log2 (N^2 / t)
    // EV_100 = log2 (N^2 / t) - log2 (S /100)
    // EV_100 = log2 (N^2 / t . 100 / S)
    return log2(sq(aperture) / shutterTime * 100 / ISO);
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
