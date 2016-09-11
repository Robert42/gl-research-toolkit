#version 450 core

#include <fullscreen-quad.vs>


out vec2 viewport_pos;

void main()
{
  viewport_pos = get_vertex_pos();

  gl_Position = vec4(viewport_pos, 0, 1);
}

