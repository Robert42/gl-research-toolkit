#include <extensions/common.glsl>
#include <scene/uniforms.glsl>

in FragmentBlock
{
  vec3 color;
}fragment;

out vec4 color;

#define VERTEX_COLOR fragment.color
#define ORANGE_COLOR vec3(1, 0.5, 0)
#define GREEN_COLOR vec3(0.5, 1, 0)
#define ERROR_COLOR vec3(1, 0, 1)
#define FRAGMENT_COORD vec3(gl_FragCoord.xy / 1024, 0)

#ifdef SHADER_DEBUG_PRINTER
#define DEBUG_FRAGMENT_COORD(fallback) vec3(debugging_buffer.fragment_coord.xy / 1024, 0)
#else
#define DEBUG_FRAGMENT_COORD(fallback) fallback
#endif

void apply_color(vec3 c)
{
  color = vec4(c, 1);
}

#define COLOR VERTEX_COLOR
void apply_color()
{
  apply_color(COLOR);
}
