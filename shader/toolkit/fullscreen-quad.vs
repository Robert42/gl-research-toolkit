#version 450 core

void main()
{
  vec2 v[4];

  v[0] = vec2( 1, -1);
  v[1] = vec2( 1,  1);
  v[2] = vec2(-1, -1);
  v[3] = vec2(-1,  1);

  gl_Position = vec4(v[gl_VertexID], 0, 1);
}
