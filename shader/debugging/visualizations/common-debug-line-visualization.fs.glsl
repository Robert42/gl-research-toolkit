in FragmentBlock
{
  vec3 color;
}fragment;

out vec4 color;

void apply_color()
{
  color = vec4(fragment.color, 1);
}
