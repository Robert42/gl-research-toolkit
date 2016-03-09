#include <glrt/glsl/layout-constants.h>
#include <glrt/glsl/debugging/printer-types.h>

#include <glrt/renderer/debugging/shader-debug-printer.h>
#include <glrt/renderer/toolkit/shader-compiler.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>


namespace glrt {
namespace renderer {
namespace debugging {


struct ShaderDebugPrinter::Chunk
{
  glm::ivec3 type;
  float z_value;
  glm::mat4 floatValues;
  glm::ivec4 integerValues;
};

struct ShaderDebugPrinter::HeaderBuffer
{
  glm::vec2 fragment_coord;
  float treshold;
  float offset;

  GLuint64 address;
};

struct ShaderDebugPrinter::ContentBuffer
{
  Chunk chunks[GLSL_DEBUGGING_LENGTH];
};


template<typename T, typename T_in>
inline void ShaderDebugPrinter::printVectorChunk(int dimension, const char* scalarName, const char* vectorPrefix, const glm::tvec4<T_in>& input, bool visualize)
{
  switch(dimension)
  {
  case 1:
    qDebug() << scalarName  << T(input.x);
    break;
  case 2:
    qDebug() << vectorPrefix  << glm::tvec2<T>(input.xy());
    break;
  case 3:
    qDebug() << vectorPrefix  << glm::tvec3<T>(input.xyz());
    if(visualize)
      positionsToDebug.append(glm::tvec3<T>(input.xyz()));
    break;
  case 4:
    qDebug() << vectorPrefix  << glm::tvec4<T>(input);
    break;
  default:
    Q_UNREACHABLE();
  }
}


inline void ShaderDebugPrinter::ShaderDebugPrinter::printUint64(const glm::ivec4& input)
{
  quint64 value;

  quint64 lower = quint32(input[0]);
  quint64 upper = quint32(input[1]);

  value = lower | (upper << 32);

  qDebug() << "uint64"  << value;
}

template<typename T, typename T_in>
inline void printMatrixChunk(glm::ivec2 dimensions, const char* matrixPrefix, const glm::tmat4x4<T_in>& input)
{
  switch(dimensions.x)
  {
  case 4:
    switch(dimensions.y)
    {
    case 4:
      qDebug() << matrixPrefix  << glm::tmat4x4<T>(input);
      break;
    default:
      Q_UNREACHABLE();
    }
  default:
    Q_UNREACHABLE();
  }
}

inline void printSphere(const glm::vec3& origin, float radius)
{
  qDebug() << "Sphere(origin="<< origin <<",   radius="<<radius<<")";
}

inline void printRect(const glm::vec3& origin, const glm::vec3& tangent1, const glm::vec3& tangent2, float half_width, float half_height)
{
  qDebug() << "Rect(origin="<< origin <<",   tangent1="<<tangent1<<",  tangent2="<<tangent2<<",  half_width="<<half_width<<",  half_height="<<half_height<<")";
}

inline void printPlane(const glm::vec3& normal, float d)
{
  qDebug() << "Plane(normal="<< normal <<",   d="<<d<<")";
}

inline void ShaderDebugPrinter::printRay(const glm::vec3& origin, const glm::vec3& direction, bool visualize)
{
  qDebug() << "Ray(origin="<< origin <<",   direction="<<direction<<")";
  if(visualize)
  {
    positionsToDebug.append(origin);

    Arrow arrow;
    arrow.from = origin;
    arrow.to = origin+direction;

    directionsToDebug.append(arrow);
  }
}


inline void ShaderDebugPrinter::printChunk(const Chunk& chunk)
{
  if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_BOOL(1)[0])
    printVectorChunk<bool>(chunk.type.y, "bool", "b", chunk.integerValues);
  else if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_INT32(1)[0])
    printVectorChunk<qint32>(chunk.type.y, "int32", "i", chunk.integerValues);
  else if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_UINT32(1)[0])
    printVectorChunk<quint32>(chunk.type.y, "uint32", "i", chunk.integerValues);
  else if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_UINT64(1)[0])
    printUint64(chunk.integerValues);
  else if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_FLOAT(1)[0])
    printVectorChunk<float>(chunk.type.y, "float", "", chunk.floatValues[0], chunk.integerValues[0]);
  else if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_MAT(1,1)[0])
    printMatrixChunk<float>(chunk.type.yz(), "", chunk.floatValues);
  else if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_SPHERE[0])
    printSphere(chunk.floatValues[0].xyz(), chunk.floatValues[0].w);
  else if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_RECT[0])
    printRect(chunk.floatValues[0].xyz(), chunk.floatValues[1].xyz(), chunk.floatValues[2].xyz(), chunk.floatValues[1].w, chunk.floatValues[2].w);
  else if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_PLANE[0])
    printPlane(chunk.floatValues[0].xyz(), chunk.floatValues[0].w);
  else if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_RAY[0])
    printRay(chunk.floatValues[0].xyz(), chunk.floatValues[1].xyz(), chunk.integerValues[0]);
  else
    qCritical() << "ShaderDebugPrinter: printChunk: Unknown chunk-type " << chunk.type;
}


ShaderDebugPrinter::ShaderDebugPrinter()
  : shader(std::move(ShaderCompiler::createShaderFromFiles("visualize-debug-printing-fragment", QDir(GLRT_SHADER_DIR"/debugging/visualizations")))),
    counterBuffer(sizeof(int), gl::Buffer::UsageFlag(gl::Buffer::UsageFlag::MAP_READ|gl::Buffer::UsageFlag::MAP_WRITE), nullptr),
    headerBuffer(sizeof(HeaderBuffer), gl::Buffer::UsageFlag(gl::Buffer::UsageFlag::MAP_WRITE), nullptr),
    contentBuffer(sizeof(ContentBuffer), gl::Buffer::UsageFlag(gl::Buffer::UsageFlag::MAP_READ | gl::Buffer::UsageFlag::MAP_WRITE), nullptr),
    positionVisualization(VisualizationRenderer::debugPoints(&positionsToDebug)),
    directionVisualization(VisualizationRenderer::debugArrows(&directionsToDebug))
{
  clearScene = false;

  guiToggle.getter = [this]() -> bool {return this->active;};
  guiToggle.setter = [this](bool active) {
    this->active = active;
    QString preprocessorBlock = "#define SHADER_DEBUG_PRINTER";
    if(this->active)
    {
      ReloadableShader::globalPreprocessorBlock.insert(preprocessorBlock);
      positionVisualization.setEnabled(true);
      directionVisualization.setEnabled(true);
    }else
    {
      ReloadableShader::globalPreprocessorBlock.remove(preprocessorBlock);
      positionVisualization.setEnabled(false);
      directionVisualization.setEnabled(false);
    }
    ReloadableShader::reloadAll();
  };
}

ShaderDebugPrinter::~ShaderDebugPrinter()
{
}


void ShaderDebugPrinter::begin()
{
  if(!active || !mouse_is_pressed)
    return;

  HeaderBuffer* header = reinterpret_cast<HeaderBuffer*>(headerBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));

  header->fragment_coord = glm::vec2(mouseCoordinate);
  header->treshold = 0.5f;
  header->offset = 0.5f;
  header->address = contentBuffer.gpuBufferAddress();

  headerBuffer.Unmap();

  ContentBuffer* content = reinterpret_cast<ContentBuffer*>(contentBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  memset(content, 0, sizeof(ContentBuffer));
  contentBuffer.Unmap();

  quint32& counter = *reinterpret_cast<quint32*>(counterBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  counter = 0;
  counterBuffer.Unmap();

  headerBuffer.BindUniformBuffer(UNIFORM_BINDING_VALUE_PRINTER);
  counterBuffer.BindAtomicCounterBuffer(ATOMIC_COUNTER_BINDING_VALUE_PRINTER);
}

void ShaderDebugPrinter::end()
{
  if(!active || !mouse_is_pressed)
    return;

  // TODO: some memory barrier necessary here?

  const quint32 numberChunks = *reinterpret_cast<quint32*>(counterBuffer.Map(gl::Buffer::MapType::READ, gl::Buffer::MapWriteFlag::NONE));
  counterBuffer.Unmap();

  ContentBuffer content = *reinterpret_cast<ContentBuffer*>(contentBuffer.Map(gl::Buffer::MapType::READ, gl::Buffer::MapWriteFlag::NONE));
  contentBuffer.Unmap();

  if(numberChunks > 0)
    qDebug() << "\n\n";

  positionsToDebug.clear();
  directionsToDebug.clear();

  float min_z = INFINITY;
  for(quint32 i=0; i<numberChunks && i<GLSL_DEBUGGING_LENGTH; ++i)
    min_z = glm::min(content.chunks[i].z_value, min_z);
  for(quint32 i=0; i<numberChunks && i<GLSL_DEBUGGING_LENGTH; ++i)
    if(content.chunks[i].z_value == min_z)
      printChunk(content.chunks[i]);

  positionVisualization.update();
  directionVisualization.update();
}

void ShaderDebugPrinter::draw()
{
  if(!active)
    return;

  if(clearScene)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  positionVisualization.render();
  directionVisualization.render();

  if(!mouse_is_pressed)
    return;

  shader.Activate();
  headerBuffer.BindShaderStorageBuffer(UNIFORM_BINDING_VALUE_PRINTER);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL_RECTANGLE_NV);
  GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 3);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool ShaderDebugPrinter::handleEvents(const SDL_Event& event)
{
  if(!active)
    return false;

  switch(event.type)
  {
  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
    switch(event.button.button)
    {
    case SDL_BUTTON_LEFT:
      this->mouse_is_pressed = event.button.state == SDL_PRESSED;
      if(this->mouse_is_pressed)
        setMouseCoordinate(glm::ivec2(event.button.x, event.button.y), event.motion.windowID);
      return true;
    default:
      return false;
    }
  case SDL_MOUSEMOTION:
    if(mouse_is_pressed)
    {
      setMouseCoordinate(glm::ivec2(event.motion.x, event.motion.y), event.motion.windowID);
      return true;
    }
    return false;
  default:
    return false;
  }
}

void ShaderDebugPrinter::setMouseCoordinate(const glm::ivec2& mouseCoordinate, Uint32 windowId)
{
  glm::ivec2 windowSize;

  SDL_GetWindowSize(SDL_GetWindowFromID(windowId), &windowSize.x, &windowSize.y);

  this->mouseCoordinate = glm::ivec2(mouseCoordinate.x, windowSize.y-mouseCoordinate.y);
}


} // namespace debugging
} // namespace renderer
} // namespace glrt
