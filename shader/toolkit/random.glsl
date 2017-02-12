vec2 Hammersley(uint n, uint N)
{
  float g_2 = 0.f;
  float base = 0.5;

  for(uint k=1; k<N; k<<=1)
  {
    if((n & k) != 0)
      g_2 += base;
    base *= 0.5;
  }

  return vec2(g_2, double(n)/double(N));
}


vec2 getSample(uint i, uint sampleCount)
{
  return Hammersley(i, sampleCount);
}
