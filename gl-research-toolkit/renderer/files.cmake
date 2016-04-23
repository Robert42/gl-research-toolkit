set(SOURCE_FILES
  include/glrt/application.h
  include/glrt/gui/anttweakbar.h
  include/glrt/gui/toolbar.h
  include/glrt/renderer/debugging/debugging-posteffect.h
  include/glrt/renderer/debugging/debug-line-visualisation.h
  include/glrt/renderer/debugging/debug-mesh.h
  include/glrt/renderer/debugging/debug-renderer.h
  include/glrt/renderer/debugging/shader-debug-printer.h
  include/glrt/renderer/debugging/surface-shader-visualizations.h
  include/glrt/renderer/debugging/visualization-renderer.h
  include/glrt/renderer/declarations.h
  include/glrt/renderer/dependencies.h
  include/glrt/renderer/forward-renderer.h
  include/glrt/renderer/gl/command-list.h
  include/glrt/renderer/gl/command-list-recorder.h
  include/glrt/renderer/gl/status-capture.h
  include/glrt/renderer/gl-texture-manager.h
  include/glrt/renderer/implementation/fragmented-component-array.h
  include/glrt/renderer/implementation/fragmented-component-array.inl
  include/glrt/renderer/implementation/fragmented-light-component-array.h
  include/glrt/renderer/implementation/fragmented-light-component-array.inl
  include/glrt/renderer/implementation/fragmented-static-mesh-component-array.h
  include/glrt/renderer/implementation/fragmented-static-mesh-component-array.inl
  include/glrt/renderer/light-buffer.h
  include/glrt/renderer/material-buffer.h
  include/glrt/renderer/material-state.h
  include/glrt/renderer/sample-resource-manager.h
  include/glrt/renderer/scene-renderer.h
  include/glrt/renderer/simple-shader-storage-buffer.h
  include/glrt/renderer/simple-shader-storage-buffer.inl
  include/glrt/renderer/static-mesh-buffer.h
  include/glrt/renderer/static-mesh-buffer-manager.h
  include/glrt/renderer/static-mesh-renderer.h
  include/glrt/renderer/static-mesh-renderer.inl
  include/glrt/renderer/synced-fragmented-component-array.h
  include/glrt/renderer/synced-fragmented-component-array.inl
  include/glrt/renderer/toolkit/aligned-vector.h
  include/glrt/renderer/toolkit/managed-gl-buffer.h
  include/glrt/renderer/toolkit/managed-gl-buffer.inl
  include/glrt/renderer/toolkit/reloadable-shader.h
  include/glrt/renderer/toolkit/shader-compiler.h
  include/glrt/renderer/transformation-buffer.h
  include/glrt/renderer/voxel-buffer.cpp
  include/glrt/renderer/voxel-buffer.h
  include/glrt/renderer/voxelized-scene.h
  include/glrt/sample-application.h
  include/glrt/splashscreen-style.h
  include/glrt/system.h
  src/glrt/application.cpp
  src/glrt/gui/anttweakbar.cpp
  src/glrt/gui/toolbar.cpp
  src/glrt/renderer/debugging/debugging-posteffect.cpp
  src/glrt/renderer/debugging/debug-line-visualisation.cpp
  src/glrt/renderer/debugging/debug-mesh.cpp
  src/glrt/renderer/debugging/debug-renderer.cpp
  src/glrt/renderer/debugging/shader-debug-printer.cpp
  src/glrt/renderer/debugging/surface-shader-visualizations.cpp
  src/glrt/renderer/debugging/visualization-renderer.cpp
  src/glrt/renderer/forward-renderer.cpp
  src/glrt/renderer/gl/command-list.cpp
  src/glrt/renderer/gl/command-list-recorder.cpp
  src/glrt/renderer/gl/status-capture.cpp
  src/glrt/renderer/gl-texture-manager.cpp
  src/glrt/renderer/light-buffer.cpp
  src/glrt/renderer/material-buffer.cpp
  src/glrt/renderer/material-state.cpp
  src/glrt/renderer/sample-application.cpp
  src/glrt/renderer/sample-resource-manager.cpp
  src/glrt/renderer/scene-renderer.cpp
  src/glrt/renderer/static-mesh-buffer.cpp
  src/glrt/renderer/static-mesh-buffer-manager.cpp
  src/glrt/renderer/static-mesh-renderer.cpp
  src/glrt/renderer/toolkit/reloadable-shader.cpp
  src/glrt/renderer/toolkit/shader-compiler.cpp
  src/glrt/renderer/transformation-buffer.cpp
  src/glrt/renderer/voxelized-scene.cpp
  src/glrt/splashscreen-style.cpp
  src/glrt/system.cpp
)
