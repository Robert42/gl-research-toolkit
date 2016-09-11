vec2 get_vertex_pos()
{
  vec2 v[4];

  v[0] = vec2( 1, -1);
  v[1] = vec2( 1,  1);
  v[2] = vec2(-1, -1);
  v[3] = vec2(-1,  1);
  
  return v[gl_VertexID];
}
