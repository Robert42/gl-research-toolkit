cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

add_custom_target(
  gl-research-external-shader
  SOURCES
  cone-tracing/cone-occlusion.glsl
  distance-fields/debugging.glsl
  mrp/mrp.glsl
  openvdb/triangle-distance.glsl
  pbs/brdf.glsl
  pbs/camera.glsl
  pbs/light.glsl
  pbs/luminance-for-luminous-power.glsl
  pbs/material.glsl
  pbs/pbs.glsl
  pbs/roughness-adaptation.glsl
  pbs/structs.glsl
)
