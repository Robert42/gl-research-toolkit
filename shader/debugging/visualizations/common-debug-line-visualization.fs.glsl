#include <extensions/common.glsl>
#include <debugging/print.glsl>

in FragmentBlock
{
  vec3 color;
}fragment;

out vec4 color;

void apply_color()
{
#define VERTEX_COLOR fragment.color
#define ORANGE_COLOR vec3(1, 0.5, 0)
#define GREEN_COLOR vec3(0.5, 1, 0)
#define ERROR_COLOR vec3(1, 0, 1)
#define FRAGMENT_COORD vec3(gl_FragCoord.xy / 1024, 0)

#ifdef SHADER_DEBUG_PRINTER
#define DEBUG_FRAGMENT_COORD vec3(debugging_buffer.fragment_coord.xy / 1024, 0)
#else
#define DEBUG_FRAGMENT_COORD ERROR_COLOR
#endif

#define COLOR DEBUG_FRAGMENT_COORD

  color = vec4(COLOR, 1);
}
