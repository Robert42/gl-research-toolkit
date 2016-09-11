#version 450 core

#include <fullscreen-quad.vs>

void main()
{
  gl_Position = vec4(get_vertex_pos(), 0, 1);
}
