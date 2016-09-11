#version 450 core

out vec2 viewport_pos;

void main()
{
  vec2 v[4];

  v[0] = vec2( 1, -1);
  v[1] = vec2( 1,  1);
  v[2] = vec2(-1, -1);
  v[3] = vec2(-1,  1);
  
  vec2 vertex = v[gl_VertexID];
  
  viewport_pos = vertex;

  gl_Position = vec4(vertex, 0, 1);
}
