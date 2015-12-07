#include <glrt/glsl/layout-constants.h>
#include "light-structs.glsl"

layout(binding=SHADERSTORAGE_BINDING_LIGHTS_SPHEREAREA, std140)
buffer SphereAreaLightBlock
{
  int num;
  
  SphereAreaLight lights[];
}sphere_arealights;

layout(binding=SHADERSTORAGE_BINDING_LIGHTS_RECTAREA, std140)
buffer RectAreaLightBlock
{
  int num;
  
  RectAreaLight lights[];
}rect_arealights;
