#version 450 core
#include <extensions/common.glsl>

#include <fullscreen-quad.vs.glsl>
#include <scene/uniforms.glsl>

flat out mat4 inv_projection_view_matrix;

out vec2 viewport_pos;

void main()
{
  viewport_pos = get_vertex_pos();

  inv_projection_view_matrix = inverse(scene.view_projection);

  gl_Position = vec4(viewport_pos, 0, 1);
}

