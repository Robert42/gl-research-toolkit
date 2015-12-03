// GLHelper is meant as "copy and paste"  helper class collection. As such you are encouraged to integrate it seamlessly.
// This files contains several type and function definitions that you usually want to alter for a better integration into your application.

#include <string>
#include <cassert>
#include <iostream>
#include <limits>  // for std::numeric_limits in tetxureview.hpp
#include <string.h> // for memcmp in samplerobject.h
#include <vector> // for std::vector in shaderobject.cpp
#include <algorithm> // for std::count in shaderobject.cpp

#include <QDebug>
#include <QVector>
#include <QDir>


namespace gl
{
	namespace Details
	{

    inline const char* to_c_str(const std::string& str)
    {
      return str.c_str();
    }

    inline const char* to_c_str(const char* str)
    {
      return str;
    }

  }
}


// General settings.

// If activated, Texture2D has a FromFile method which uses stbi to load images and create mipmaps.
//#define TEXTURE2D_FROMFILE_STBI

// Activates output of shader compile logs to log.
#define SHADER_COMPILE_LOGS



// Assert
#ifndef NDEBUG
#define GLHELPER_ASSERT(condition, message) do { \
	if(!(condition)) std::cerr << message; \
	assert(condition); } while(false)
#else
#define GLHELPER_ASSERT(condition, string) do { } while(false)
#endif


// Logging
#define GLHELPER_LOG_ERROR(message)		do { qCritical() << QString("Error: %0 (<%1> line %2)").arg(gl::Details::to_c_str(message)).arg(__FILE__).arg(__LINE__).toStdString().c_str(); } while(false)
#define GLHELPER_LOG_WARNING(message)	do { qWarning() << QString("Warning: %0 (<%1> line %2)").arg(gl::Details::to_c_str(message)).arg(__FILE__).arg(__LINE__).toStdString().c_str(); } while(false)
#define GLHELPER_LOG_INFO(message)		do { qDebug() << QString("Info: %0 (<%1> line %2)").arg(gl::Details::to_c_str(message)).arg(__FILE__).arg(__LINE__).toStdString().c_str(); } while(false)



// Vector & Matrix types.
#include <glm/glm.hpp>
namespace gl
{
	typedef glm::vec2 Vec2;
	typedef glm::vec3 Vec3;
	typedef glm::vec4 Vec4;

	typedef glm::ivec2 IVec2;
	typedef glm::ivec3 IVec3;
	typedef glm::ivec4 IVec4;

	typedef glm::uvec2 UVec2;
	typedef glm::uvec3 UVec3;
	typedef glm::uvec4 UVec4;

	typedef glm::mat3x3 Mat3;
	typedef glm::mat4x4 Mat4;

  namespace Details
  {

    class ShaderIncludeDirManager final
    {
    public:
      ShaderIncludeDirManager() = delete;

      static void addIncludeDirs(const QDir& dir)
      {
        getIncludeDirs().append(dir);
      }

      static std::string expandGlobalInclude(const std::string& include_std_string)
      {
        QString include_file = QString::fromStdString(include_std_string);

        for(const QDir& dir : getIncludeDirs())
        {
          if(dir.exists(include_file))
            return dir.absoluteFilePath(include_file).toStdString();
        }

        return std::string();
      }

    private:
      static QVector<QDir>& getIncludeDirs()
      {
        static QVector<QDir> include_dirs;
        return include_dirs;
      }
    };

  }
}

// A std::vector of all include paths shaders will be looked for, if an #include<...> statement was found during parsing an glsl script
#define SHADER_EXPAND_GLOBAL_INCLUDE(x) gl::Details::ShaderIncludeDirManager::expandGlobalInclude(x)

// OpenGL header.

#include <GL/glew.h>
