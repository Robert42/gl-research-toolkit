#include <glrt/debugging/shader-debug-printer.h>

#include <glrt/toolkit/shader-compiler.h>

#include <glrt/glsl/debugging/printer-types.h>
#include <glrt/glsl/layout-constants.h>


namespace glrt {
namespace debugging {


struct Chunk
{
  glm::ivec3 type;
  float z_value;
  glm::mat4 floatValues;
  glm::ivec4 integerValues;
};

struct WholeBuffer
{
  glm::vec2 fragment_coord;
  float treshold;
  float offset;

  Chunk chunks[GLSL_DEBUGGING_LENGTH];
};


template<typename T, typename T_in>
inline void printVectorChunk(int dimension, const char* scalarName, const char* vectorPrefix, const glm::tvec4<T_in>& input)
{
  switch(dimension)
  {
  case 1:
    qDebug() << scalarName  << " " << T(input.x);
    break;
  case 2:
    qDebug() << vectorPrefix  << "vec2 " << glm::tvec2<T>(input.xy());
    break;
  case 3:
    qDebug() << vectorPrefix  << "vec3 " << glm::tvec3<T>(input.xyz());
    break;
  case 4:
    qDebug() << vectorPrefix  << "vec4 " << glm::tvec4<T>(input);
    break;
  default:
    Q_UNREACHABLE();
  }
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
      qDebug() << matrixPrefix  << "mat4x4 " << glm::tmat4x4<T>(input);
      break;
    default:
      Q_UNREACHABLE();
    }
  default:
    Q_UNREACHABLE();
  }
}

inline void printRay(const glm::vec3& origin, const glm::vec3& direction)
{
  qDebug() << "Ray(origin="<< origin <<",   direction="<<direction<<")";
}


inline void printChunk(const Chunk& chunk)
{
  if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_BOOL(1)[0])
    printVectorChunk<bool>(chunk.type.y, "bool", "b", chunk.integerValues);
  else if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_INT(1)[0])
    printVectorChunk<int>(chunk.type.y, "int", "i", chunk.integerValues);
  else if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_FLOAT(1)[0])
    printVectorChunk<float>(chunk.type.y, "float", "", chunk.floatValues[0]);
  else if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_MAT(1,1)[0])
    printMatrixChunk<float>(chunk.type.yz(), "", chunk.floatValues);
  else if(chunk.type[0] == glm::GLSL_DEBUGGING_TYPE_RAY[0])
    printRay(chunk.floatValues[0].xyz(), chunk.floatValues[1].xyz());
  else
    qCritical() << "ShaderDebugPrinter: printChunk: Unknown chunk-type " << chunk.type;
}


ShaderDebugPrinter::ShaderDebugPrinter()
  : shader(std::move(ShaderCompiler::createShaderFromFiles("visualize-debug-printing-fragment", QDir(GLRT_SHADER_DIR"/debugging/visualizations")))),
    counterBuffer(sizeof(int), gl::Buffer::UsageFlag(gl::Buffer::UsageFlag::MAP_READ|gl::Buffer::UsageFlag::MAP_WRITE), nullptr),
    buffer(sizeof(WholeBuffer), gl::Buffer::UsageFlag(gl::Buffer::UsageFlag::MAP_READ|gl::Buffer::UsageFlag::MAP_WRITE), nullptr)
{
  guiToggle.getter = [this]() -> bool {return this->active;};
  guiToggle.setter = [this](bool active) {
    this->active = active;
  };
}

ShaderDebugPrinter::~ShaderDebugPrinter()
{
}


void ShaderDebugPrinter::begin()
{
  if(!active || !mouse_is_pressed)
    return;

  WholeBuffer* whole_buffer = reinterpret_cast<WholeBuffer*>(buffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));

  memset(whole_buffer, 0, sizeof(WholeBuffer));

  whole_buffer->fragment_coord = glm::vec2(mouseCoordinate);
  whole_buffer->treshold = 0.5f;
  whole_buffer->offset = 0.5f;

  buffer.Unmap();

  quint32& counter = *reinterpret_cast<quint32*>(counterBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  counter = 0;
  counterBuffer.Unmap();

  buffer.BindShaderStorageBuffer(SHADERSTORAGE_BINDING_VALUE_PRINTER);
  counterBuffer.BindAtomicCounterBuffer(ATOMIC_COUNTER_BINDING_VALUE_PRINTER);
}

void ShaderDebugPrinter::end()
{
  if(!active || !mouse_is_pressed)
    return;

  // TODO: some memory barrier necessary here?

  const quint32 numberChunks = *reinterpret_cast<quint32*>(counterBuffer.Map(gl::Buffer::MapType::READ, gl::Buffer::MapWriteFlag::NONE));
  counterBuffer.Unmap();

  WholeBuffer whole_buffer = *reinterpret_cast<WholeBuffer*>(buffer.Map(gl::Buffer::MapType::READ, gl::Buffer::MapWriteFlag::NONE));
  buffer.Unmap();

  if(numberChunks > 0)
    qDebug() << "\n\n";

  float min_z = INFINITY;
  for(quint32 i=0; i<numberChunks && i<GLSL_DEBUGGING_LENGTH; ++i)
    min_z = glm::min(whole_buffer.chunks[i].z_value, min_z);
  for(quint32 i=0; i<numberChunks && i<GLSL_DEBUGGING_LENGTH; ++i)
    if(whole_buffer.chunks[i].z_value == min_z)
      printChunk(whole_buffer.chunks[i]);
}

void ShaderDebugPrinter::drawCross()
{
  if(!active || !mouse_is_pressed)
    return;

  shader.Activate();
  buffer.BindShaderStorageBuffer(SHADERSTORAGE_BINDING_VALUE_PRINTER);
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
} // namespace glrt
