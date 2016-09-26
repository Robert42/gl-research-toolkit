#include <glrt/renderer/debugging/debugging-posteffect.h>
#include <glrt/renderer/static-mesh-buffer.h>
#include <glrt/system.h>
#include <glrt/glsl/math-cpp.h>
#include <glrt/glsl/layout-constants.h>
#include <glrt/renderer/gl/shader-type.h>
#include <QTimer>

namespace glrt {
namespace renderer {
namespace debugging {


class DebuggingPosteffect::Renderer : public DebugRenderer::Implementation, public ReloadableShader::Listener
{
public:
  gl::StatusCapture statusCapture;
  gl::CommandList commandList;
  bool depthTest;
  bool needRerecording = true;
  padding<byte, 2> _padding;
  gl::Buffer fragmentUniformBuffer;

  Renderer(bool depthTest);
  ~Renderer();

  virtual void activateShader() = 0;

  void enqueueRerecordingCommandList();
  void render() override;

private:
  void recordCommandList();
  void allShadersReloaded() override;
};


DebuggingPosteffect::Renderer::Renderer(bool depthTest)
  : depthTest(depthTest)
{
  enqueueRerecordingCommandList();

  // This really makes no sense. Part of the uglies workaround of my life :(
  // For some reason activating the shader debug printer prevents the debuggig posteffect from wobbling..
  debugging::ShaderDebugPrinter::workaround71++;
}

DebuggingPosteffect::Renderer::~Renderer()
{
  debugging::ShaderDebugPrinter::workaround71--;
}

void DebuggingPosteffect::Renderer::recordCommandList()
{
  if(!needRerecording)
    return;
  needRerecording = false;

  Q_ASSERT(!renderingData.isNull());

  const glm::ivec2 videoResolution = glrt::System::windowSize();
  gl::FramebufferObject& framebuffer = renderingData->framebuffer;
  glrt::renderer::Renderer& renderer = renderingData->renderer;

  const int bindingIndex = 0;

  framebuffer.Bind(false);
  activateShader();
  GL_CALL(glVertexAttribFormatNV, bindingIndex, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float));
  GL_CALL(glEnableVertexAttribArray, bindingIndex);
  if(depthTest)
    GL_CALL(glEnable, GL_DEPTH_TEST);
  else
    GL_CALL(glDisable, GL_DEPTH_TEST);
  statusCapture = gl::StatusCapture::capture(gl::StatusCapture::Mode::TRIANGLES);
  framebuffer.BindBackBuffer();
  gl::Program::useNone();
  GL_CALL(glDisable, GL_DEPTH_TEST);
  GL_CALL(glDisableVertexAttribArray, bindingIndex);


  glm::ivec2 tokenRange;
  gl::CommandListRecorder segment;

  segment.beginTokenList();
  segment.append_token_Viewport(glm::uvec2(0), glm::uvec2(videoResolution));
  segment.append_token_AttributeAddress(bindingIndex, renderingData->vertexBuffer.gpuBufferAddress());
  segment.append_token_UniformAddress(UNIFORM_BINDING_SCENE_BLOCK, gl::ShaderType::VERTEX, renderer.sceneUniformAddress());
  segment.append_token_UniformAddress(UNIFORM_BINDING_SCENE_BLOCK, gl::ShaderType::FRAGMENT, renderer.sceneUniformAddress());
  segment.append_token_UniformAddress(UNIFORM_BINDING_POSTEFFECTVISUALIZATION_BLOCK, gl::ShaderType::FRAGMENT, renderingData->uniformBuffer.gpuBufferAddress());
  renderer.debugPrinter.recordBinding(segment);
  if(fragmentUniformBuffer.GetSize() != 0)
    segment.append_token_UniformAddress(UNIFORM_BINDING_DEBUG_POSTEFFECT_FRAGMENT, gl::ShaderType::FRAGMENT, fragmentUniformBuffer.gpuBufferAddress());
  segment.append_token_DrawArrays(4, 0, gl::CommandListRecorder::Strip::STRIP);
  tokenRange = segment.endTokenList();

  segment.append_drawcall(tokenRange, &statusCapture, &framebuffer);

  commandList = gl::CommandListRecorder::compile(std::move(segment));
}

void DebuggingPosteffect::Renderer::enqueueRerecordingCommandList()
{
  needRerecording = true;
  QTimer::singleShot(0, this, &DebuggingPosteffect::Renderer::recordCommandList);
}

void DebuggingPosteffect::Renderer::allShadersReloaded()
{
  enqueueRerecordingCommandList();
}


void DebuggingPosteffect::Renderer::render()
{
  Q_ASSERT(!needRerecording);

  Q_ASSERT(!renderingData.isNull());
  renderingData->updateUniforms();

  commandList.call();
}


class SingleShader : public DebuggingPosteffect::Renderer
{
public:
  ReloadableShader shader;

  SingleShader(const QSet<QString>& debug_posteffect_preprocessor, const QString& name, bool depthTest);

  void activateShader() override;
};


class OrangeSphere : public SingleShader
{
public:
  OrangeSphere(const QSet<QString>& debug_posteffect_preprocessor, const glm::vec3& origin, float radius, bool depthTest);
};


class HighlightUnconveiledNegativeDistances : public SingleShader
{
public:
  HighlightUnconveiledNegativeDistances(const QSet<QString>& debug_posteffect_preprocessor, bool depthTest);
};

class HighlightVoxelGrids : public SingleShader
{
public:
  HighlightVoxelGrids(const QSet<QString>& debug_posteffect_preprocessor, bool depthTest);
};

class CubicVoxelRaymarch : public SingleShader
{
public:
  CubicVoxelRaymarch(const QSet<QString>& debug_posteffect_preprocessor, bool depthTest);
};

class DistanceFieldRaymarch : public SingleShader
{
public:
  DistanceFieldRaymarch(const QSet<QString>& debug_posteffect_preprocessor, bool depthTest);
};

class RaymarchBoundingSpheresAsDistanceField : public SingleShader
{
public:
  RaymarchBoundingSpheresAsDistanceField(const QSet<QString>& debug_posteffect_preprocessor, bool depthTest);
};

class GlobalDistanceFieldRaymarch : public SingleShader
{
public:
  GlobalDistanceFieldRaymarch(const QSet<QString>& debug_posteffect_preprocessor, bool depthTest);
};


SingleShader::SingleShader(const QSet<QString>& debug_posteffect_preprocessor, const QString& name, bool depthTest)
  : Renderer(depthTest),
    shader(name,
           QDir(GLRT_SHADER_DIR"/debugging/posteffects"),
           debug_posteffect_preprocessor)
{
}

void SingleShader::activateShader()
{
  shader.glProgram.use();
}


OrangeSphere::OrangeSphere(const QSet<QString>& debug_posteffect_preprocessor, const glm::vec3& origin, float radius, bool depthTest)
  : SingleShader(debug_posteffect_preprocessor, "orange-sphere", depthTest)
{
  glsl::Sphere sphere;

  sphere.origin = origin;
  sphere.radius = radius;

  fragmentUniformBuffer = gl::Buffer(sizeof(glsl::Sphere), gl::Buffer::IMMUTABLE, &sphere);
}


HighlightVoxelGrids::HighlightVoxelGrids(const QSet<QString>& debug_posteffect_preprocessor, bool depthTest)
  : SingleShader(debug_posteffect_preprocessor, "highlight-voxel-bounding-rect", depthTest)
{
}


HighlightUnconveiledNegativeDistances::HighlightUnconveiledNegativeDistances(const QSet<QString>& debug_posteffect_preprocessor, bool depthTest)
  : SingleShader(debug_posteffect_preprocessor, "highlight-unconceiled-negative-distance", depthTest)
{
}


CubicVoxelRaymarch::CubicVoxelRaymarch(const QSet<QString>& debug_posteffect_preprocessor, bool depthTest)
  : SingleShader(debug_posteffect_preprocessor, "show-distancefield-as-cubic-voxel", depthTest)
{
}


DistanceFieldRaymarch::DistanceFieldRaymarch(const QSet<QString>& debug_posteffect_preprocessor, bool depthTest)
  : SingleShader(debug_posteffect_preprocessor, "raymarch-distancefield", depthTest)
{
}


RaymarchBoundingSpheresAsDistanceField::RaymarchBoundingSpheresAsDistanceField(const QSet<QString>& debug_posteffect_preprocessor, bool depthTest)
  : SingleShader(debug_posteffect_preprocessor, "show-bounding-spheres-as-distancefield", depthTest)
{
}


GlobalDistanceFieldRaymarch::GlobalDistanceFieldRaymarch(const QSet<QString>& debug_posteffect_preprocessor, bool depthTest)
  : SingleShader(debug_posteffect_preprocessor, "raymarch-global-distancefield", depthTest)
{
}


DebugRenderer DebuggingPosteffect::orangeSphere(const QSet<QString>& debug_posteffect_preprocessor, const glm::vec3& origin, float radius, bool depthTest)
{
  padding<byte, 7> padding;
  return DebugRenderer::ImplementationFactory([debug_posteffect_preprocessor, depthTest, padding, origin, radius](){return new OrangeSphere(debug_posteffect_preprocessor, origin, radius, depthTest);});
}

DebugRenderer DebuggingPosteffect::voxelGridHighlightUnconveiledNegativeDistances(const QSet<QString>& debug_posteffect_preprocessor, bool depthTest)
{
  padding<byte, 7> padding;
  return DebugRenderer::ImplementationFactory([debug_posteffect_preprocessor, depthTest, padding](){return new HighlightUnconveiledNegativeDistances(debug_posteffect_preprocessor, depthTest);});
}

DebugRenderer DebuggingPosteffect::voxelGridBoundingBox(const QSet<QString>& debug_posteffect_preprocessor, bool depthTest)
{
  padding<byte, 7> padding;
  return DebugRenderer::ImplementationFactory([debug_posteffect_preprocessor, depthTest, padding](){return new HighlightVoxelGrids(debug_posteffect_preprocessor, depthTest);});
}

DebugRenderer DebuggingPosteffect::voxelGridCubicRaymarch(const QSet<QString>& debug_posteffect_preprocessor, bool mixWithScene)
{
  padding<byte, 7> padding;
  return DebugRenderer::ImplementationFactory([debug_posteffect_preprocessor, mixWithScene, padding](){return new CubicVoxelRaymarch(debug_posteffect_preprocessor, mixWithScene);});
}

DebugRenderer DebuggingPosteffect::distanceFieldRaymarch(const QSet<QString>& debug_posteffect_preprocessor, bool mixWithScene)
{
  padding<byte, 7> padding;
  return DebugRenderer::ImplementationFactory([debug_posteffect_preprocessor, mixWithScene, padding](){return new DistanceFieldRaymarch(debug_posteffect_preprocessor, mixWithScene);});
}

DebugRenderer DebuggingPosteffect::raymarchBoundingSpheresAsDistanceField(const QSet<QString>& debug_posteffect_preprocessor, bool mixWithScene)
{
  padding<byte, 7> padding;
  return DebugRenderer::ImplementationFactory([debug_posteffect_preprocessor, mixWithScene, padding](){return new RaymarchBoundingSpheresAsDistanceField(debug_posteffect_preprocessor, mixWithScene);});
}

DebugRenderer DebuggingPosteffect::globalDistanceFieldRaymarch(const QSet<QString>& debug_posteffect_preprocessor, bool mixWithScene)
{
  padding<byte, 7> padding;
  return DebugRenderer::ImplementationFactory([debug_posteffect_preprocessor, mixWithScene, padding](){return new GlobalDistanceFieldRaymarch(debug_posteffect_preprocessor, mixWithScene);});
}

QSharedPointer<DebuggingPosteffect::SharedRenderingData> DebuggingPosteffect::renderingData;

void DebuggingPosteffect::init(gl::FramebufferObject* framebuffer, glrt::renderer::Renderer* renderer)
{
  Q_ASSERT(renderingData.isNull());

  renderingData = QSharedPointer<SharedRenderingData>(new SharedRenderingData(framebuffer, renderer));
}

void DebuggingPosteffect::deinit()
{
  renderingData.clear();
}

DebuggingPosteffect::SharedRenderingData::SharedRenderingData(gl::FramebufferObject* framebuffer, glrt::renderer::Renderer* renderer)
  : framebuffer(*framebuffer),
    renderer(*renderer)
{
  Q_ASSERT(framebuffer);
  Q_ASSERT(renderer);

  float min_coord = -1;
  float max_coord = 1;
  const std::vector<float> positions = {min_coord, min_coord,
                                        min_coord, max_coord,
                                        max_coord, min_coord,
                                        max_coord, max_coord};

  vertexBuffer = gl::Buffer(sizeof(float)*8, gl::Buffer::UsageFlag::IMMUTABLE, positions.data());
  uniformBuffer = gl::Buffer(sizeof(debugging::PosteffectVisualizationDataBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr);
}

DebuggingPosteffect::SharedRenderingData::~SharedRenderingData()
{
}

void DebuggingPosteffect::SharedRenderingData::updateUniforms()
{
  PosteffectVisualizationDataBlock* data_block = reinterpret_cast<PosteffectVisualizationDataBlock*>(uniformBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  *data_block = renderer.debugPosteffect;
  uniformBuffer.Unmap();
}


} // namespace debugging
} // namespace renderer
} // namespace glrt
