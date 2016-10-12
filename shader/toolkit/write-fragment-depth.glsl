float FragCoord_z_toFragDepth(float z)
{
  // TODO check for performance improvement by leaving out gl_DepthRange.diff and gl_DepthRange.near
  return (0.5 * z + 0.5) * gl_DepthRange.diff + gl_DepthRange.near;
}

void write_fragment_depth(float z)
{
  gl_FragDepth = FragCoord_z_toFragDepth(z);
}
