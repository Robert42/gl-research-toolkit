#version 450 core

#include "common-debug-line-visualization.fs.glsl"

void test_project_line(vec3 v)
{
  PRINT_VALUE((scene.view_projection * vec4(v, 1)).xyz);
}

void main()
{
  test_project_line(vec3(0, 0, 0));
  test_project_line(vec3(0, 1, 0));
  test_project_line(vec3(1, 0, 0));
  test_project_line(vec3(1, 1, 0));
  apply_color(DEBUG_FRAGMENT_COORD(ORANGE_COLOR));
}
