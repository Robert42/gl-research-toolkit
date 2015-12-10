#include <glrt/debugging/shader-debug-printer.h>

#include <glrt/toolkit/shader-compiler.h>

#include <glrt/glsl/debugging/printer-types.h>
#include <glrt/glsl/layout-constants.h>


namespace glrt {
namespace debugging {


struct Chunk
{
  glm::ivec4 type;
  glm::mat4 floatValues;
  glm::ivec4 integerValues;
};

struct WholeBuffer
{
  glm::vec2 fragment_coord;
  float treshold;
  int numberChunks;
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
}


ShaderDebugPrinter::ShaderDebugPrinter()
  : shader(std::move(ShaderCompiler::createShaderFromFiles("visualize-debug-printing-fragment", QDir(GLRT_SHADER_DIR"/debugging/visualizations")))),
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
  glm::ivec2 mouseCoordinate;
  SDL_GetRelativeMouseState(&mouseCoordinate.x, &mouseCoordinate.y);

  WholeBuffer* whole_buffer = reinterpret_cast<WholeBuffer*>(buffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));

  memset(whole_buffer, 0, sizeof(WholeBuffer));

  whole_buffer->fragment_coord = glm::vec2(mouseCoordinate);
  whole_buffer->treshold = 0.5f;

  buffer.Unmap();

  buffer.BindShaderStorageBuffer(SHADERSTORAGE_BINDING_VALUE_PRINTER);
}

void ShaderDebugPrinter::end()
{
  if(!active || !mouse_is_pressed)
    return;

  WholeBuffer whole_buffer = *reinterpret_cast<WholeBuffer*>(buffer.Map(gl::Buffer::MapType::READ, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  buffer.Unmap();

  if(whole_buffer.numberChunks > 0)
    qDebug() << "\n\n";

  for(int i=0; i<whole_buffer.numberChunks; ++i)
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
      return true;
    default:
      return false;
    }
  default:
    return false;
  }
}


} // namespace debugging
} // namespace glrt
