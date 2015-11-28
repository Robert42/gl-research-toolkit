cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

add_custom_target(
  gl-research-toolkit-shader
  SOURCES
  samples/single-mesh/debugging.glsl
  samples/single-mesh/plain-color.fs
  samples/single-mesh/plain-color.vs
  toolkit/debugging/normal.glsl
)
