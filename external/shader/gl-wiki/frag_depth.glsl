// https://www.opengl.org/wiki/Compute_eye_space_from_window_space#From_gl_FragCoord
float FragCoord_z_toFragDepth(float z)
{
  return (2. * z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
}