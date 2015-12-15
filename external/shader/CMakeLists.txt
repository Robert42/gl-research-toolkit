cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

add_custom_target(
  gl-research-external-shader
  SOURCES
  pbs/brdf.glsl
  pbs/light.glsl
  pbs/material.glsl
  pbs/pbs.glsl
)
