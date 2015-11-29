cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

add_custom_target(
  gl-research-toolkit-shader
  SOURCES
  toolkit/debugging/normal.glsl
  materials/masked-meshes.vs
  materials/masked-meshes.fs
  materials/plain-color-materials.vs
  materials/plain-color-materials.fs
  materials/textured-meshes.vs
  materials/textured-meshes.fs
  materials/transparent-meshes.vs
  materials/transparent-meshes.fs
)
