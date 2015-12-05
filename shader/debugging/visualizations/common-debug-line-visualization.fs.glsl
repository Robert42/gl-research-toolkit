in FagmentBlock
{
  vec3 position;
  float parameter1;
  vec3 color;
  float parameter2;
}fragment;

out vec4 color;

void apply_color()
{
  color = vec4(fragment.color, 1);
}
