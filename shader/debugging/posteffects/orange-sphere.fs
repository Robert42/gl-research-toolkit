#version 450 core

out vec4 color;
void main()
{
  color = vec4(1, 0.5, 0, 1);
  gl_FragDepth = 0.99;
}