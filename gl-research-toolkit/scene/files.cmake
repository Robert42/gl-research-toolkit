set(SOURCE_FILES
  include/glrt/scene/aabb.h
  include/glrt/scene/camera-component.h
  include/glrt/scene/camera-parameter.h
  include/glrt/scene/collect-scene-data.h
  include/glrt/scene/collect-scene-data.inl
  include/glrt/scene/component-decorator.h
  include/glrt/scene/component-decorator.inl
  include/glrt/scene/coord-frame.h
  include/glrt/scene/coord-frame.inl
  include/glrt/scene/declarations.h
  include/glrt/scene/fps-debug-controller.h
  include/glrt/scene/global-coord-updater.h
  include/glrt/scene/input-handler.h
  include/glrt/scene/light-component.h
  include/glrt/scene/node.h
  include/glrt/scene/node.inl
  include/glrt/scene/resources/asset-converter.h
  include/glrt/scene/resources/declarations.h
  include/glrt/scene/resources/light-source.h
  include/glrt/scene/resources/material.h
  include/glrt/scene/resources/material.inl
  include/glrt/scene/resources/resource-index.h
  include/glrt/scene/resources/resource-index.inl
  include/glrt/scene/resources/resource-manager.h
  include/glrt/scene/resources/resource-manager.inl
  include/glrt/scene/resources/static-mesh-file.h
  include/glrt/scene/resources/static-mesh.h
  include/glrt/scene/resources/static-mesh.inl
  include/glrt/scene/resources/static-mesh-loader.h
  include/glrt/scene/resources/texture-file.h
  include/glrt/scene/resources/texture.h
  include/glrt/scene/resources/texture-manager.h
  include/glrt/scene/resources/texture-sampler.h
  include/glrt/scene/resources/utilities/gl-texture.h
  include/glrt/scene/resources/voxel-data.h
  include/glrt/scene/resources/voxel-file.h
  include/glrt/scene/resources/voxelizer.h
  include/glrt/scene/scene-data.h
  include/glrt/scene/scene.h
  include/glrt/scene/scene.inl
  include/glrt/scene/scene-layer.h
  include/glrt/scene/static-mesh-component.h
  include/glrt/scene/tick-manager.cpp
  include/glrt/scene/tick-manager.h
  include/glrt/scene/voxel-data-component.h
  src/glrt/scene/aabb.cpp
  src/glrt/scene/camera-component.cpp
  src/glrt/scene/camera-parameter.cpp
  src/glrt/scene/collect-scene-data.cpp
  src/glrt/scene/coord-frame.cpp
  src/glrt/scene/fps-debug-controller.cpp
  src/glrt/scene/global-coord-updater.cpp
  src/glrt/scene/input-handler.cpp
  src/glrt/scene/light-component.cpp
  src/glrt/scene/node.cpp
  src/glrt/scene/resources/asset-converter.cpp
  src/glrt/scene/resources/asset-converter-should-reimport.cpp
  src/glrt/scene/resources/cpuvoxelizerimplementation.cpp
  src/glrt/scene/resources/cpuvoxelizerimplementation.h
  src/glrt/scene/resources/light-source.cpp
  src/glrt/scene/resources/material.cpp
  src/glrt/scene/resources/resource-index.cpp
  src/glrt/scene/resources/resource-manager.cpp
  src/glrt/scene/resources/static-mesh.cpp
  src/glrt/scene/resources/static-mesh-file.cpp
  src/glrt/scene/resources/static-mesh-loader.cpp
  src/glrt/scene/resources/texture.cpp
  src/glrt/scene/resources/texture-file.cpp
  src/glrt/scene/resources/texture-manager.cpp
  src/glrt/scene/resources/texture-sampler.cpp
  src/glrt/scene/resources/utilities/gl-texture.cpp
  src/glrt/scene/resources/voxel-data.cpp
  src/glrt/scene/resources/voxel-file.cpp
  src/glrt/scene/resources/voxelizer.cpp
  src/glrt/scene/scene.cpp
  src/glrt/scene/scene-data.cpp
  src/glrt/scene/scene-layer.cpp
  src/glrt/scene/static-mesh-component.cpp
  src/glrt/scene/voxel-data-component.cpp
)
