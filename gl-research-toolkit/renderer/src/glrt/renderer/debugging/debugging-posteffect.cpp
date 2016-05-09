#include <glrt/renderer/debugging/debugging-posteffect.h>
#include <glrt/renderer/static-mesh-buffer.h>
#include <glrt/system.h>
#include <glrt/glsl/math.h>
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
  gl::ShaderObject::Deactivate();
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

  SingleShader(const QString& name, bool depthTest);

  void activateShader() override;
};


class OrangeSphere : public SingleShader
{
public:
  OrangeSphere(const glm::vec3& origin, float radius, bool depthTest);
};


class HighlightVoxelGrids : public SingleShader
{
public:
  HighlightVoxelGrids(bool depthTest);
};

class CubicVoxelRaymarch : public SingleShader
{
public:
  CubicVoxelRaymarch(bool depthTest);
};

class DistanceFieldRaymarch : public SingleShader
{
public:
  DistanceFieldRaymarch(bool depthTest);
};


SingleShader::SingleShader(const QString& name, bool depthTest)
  : Renderer(depthTest),
    shader(name,
           QDir(GLRT_SHADER_DIR"/debugging/posteffects"))
{
}

void SingleShader::activateShader()
{
  shader.shaderObject.Activate();
}


OrangeSphere::OrangeSphere(const glm::vec3& origin, float radius, bool depthTest)
  : SingleShader("orange-sphere", depthTest)
{
  glsl::Sphere sphere;

  sphere.origin = origin;
  sphere.radius = radius;

  fragmentUniformBuffer = std::move(gl::Buffer(sizeof(glsl::Sphere), gl::Buffer::IMMUTABLE, &sphere));
}


HighlightVoxelGrids::HighlightVoxelGrids(bool depthTest)
  : SingleShader("highlight-voxel-bounding-rect", depthTest)
{
}


CubicVoxelRaymarch::CubicVoxelRaymarch(bool depthTest)
  : SingleShader("show-distancefield-as-cubic-voxel", depthTest)
{
}


DistanceFieldRaymarch::DistanceFieldRaymarch(bool depthTest)
  : SingleShader("raymarch-distancefield", depthTest)
{
}


DebugRenderer DebuggingPosteffect::orangeSphere(const glm::vec3& origin, float radius, bool depthTest)
{
  padding<byte, 3> padding;
  return DebugRenderer::ImplementationFactory([origin, radius, depthTest, padding](){return new OrangeSphere(origin, radius, depthTest);});
}

DebugRenderer DebuggingPosteffect::voxelGridBoundingBox(bool depthTest)
{
  padding<byte, 3> padding;
  return DebugRenderer::ImplementationFactory([depthTest, padding](){return new HighlightVoxelGrids(depthTest);});
}

DebugRenderer DebuggingPosteffect::voxelGridCubicRaymarch(bool mixWithScene)
{
  padding<byte, 3> padding;
  return DebugRenderer::ImplementationFactory([mixWithScene, padding](){return new CubicVoxelRaymarch(mixWithScene);});
}

DebugRenderer DebuggingPosteffect::distanceFieldRaymarch(bool mixWithScene)
{
  padding<byte, 3> padding;
  return DebugRenderer::ImplementationFactory([mixWithScene, padding](){return new DistanceFieldRaymarch(mixWithScene);});
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

  vertexBuffer = std::move(gl::Buffer(sizeof(float)*8, gl::Buffer::UsageFlag::IMMUTABLE, positions.data()));
  uniformBuffer = std::move(gl::Buffer(sizeof(debugging::PosteffectVisualizationDataBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr));
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
