cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

add_custom_target(
  gl-research-external-shader
  SOURCES
  mrp/mrp.glsl
  pbs/brdf.glsl
  pbs/camera.glsl
  pbs/light.glsl
  pbs/material.glsl
  pbs/pbs.glsl
  pbs/structs.glsl
)
