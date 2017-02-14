// source https://en.wikipedia.org/w/index.php?title=Low-discrepancy_sequence&oldid=731052708#Hammersley_set
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


/*
@article{wong1997sampling,
  title={Sampling with Hammersley and Halton points},
  author={Wong, Tien-Tsin and Luk, Wai-Shing and Heng, Pheng-Ann},
  journal={Journal of graphics tools},
  volume={2},
  number={2},
  pages={9--24},
  year={1997},
  publisher={Taylor \& Francis}
}

Wong, T.T., Luk, W.S. and Heng, P.A., 1997. Sampling with Hammersley and Halton points. Journal of graphics tools, 2(2), pp.9-24.
*/
vec2 uniform_normalized_to_cylindric(vec2 u)
{
  return vec2(u.x * 2 * pi, u.y*2. - 1.);
}

vec3 uniform_normalized_to_sphere(vec2 u)
{
  u = uniform_normalized_to_cylindric(u);

  float theta = u.x;
  float t = u.y;

  float f = sqrt(1 - sq(t));
  return vec3(f * cos(theta), f*sin(theta), t);
}

vec3 getSphereSample(uint i, uint sampleCount)
{
  return uniform_normalized_to_sphere(getSample(i, sampleCount));
}
