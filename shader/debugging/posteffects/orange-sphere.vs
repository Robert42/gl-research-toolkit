#version 450 core

in layout(location=0) vec2 position;

void main()
{
  gl_Position = vec4(position.x, position.y, 0, 1);
}
