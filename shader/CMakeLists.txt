cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

add_custom_target(
  gl-research-toolkit-shader
  SOURCES
  common-with-cpp/glrt/glsl/layout-constants.h
  materials/implementation/common-implementation.vs.glsl
  materials/implementation/forward-lighting.fs.glsl
  materials/implementation/input-block.fs.glsl
  materials/implementation/material-implementation.fs.glsl
  materials/implementation/output-block.vs.glsl
  materials/implementation/texture-implementation.fs.glsl
  materials/masked-meshes.fs
  materials/masked-meshes.vs
  materials/plain-color-materials.fs
  materials/plain-color-materials.vs
  materials/textured-meshes.fs
  materials/textured-meshes.vs
  materials/transparent-meshes.fs
  materials/transparent-meshes.vs
  toolkit/debugging/normal.glsl
)
