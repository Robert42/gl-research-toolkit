#ifndef GLRT_DEPENDENCIES_H
#define GLRT_DEPENDENCIES_H

#include <GL/glew.h>

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <AntTweakBar.h>

#include <QDebug>
#include <QString>
#include <QDir>
#include <QObject>
#include <QSharedPointer>

#include <iostream>
#include <memory>
#include <vector>


namespace glrt {

class Exception final
{
public:
  Exception(const char* file, int line, const char* function, const QString& message);
};

} // namespace glrt

#define GLRT_EXCEPTION(message) Exception(__FILE__, __LINE__, __FUNCTION__, message)

#define __CALL_SDL(x, action) if(!(x)){qCritical() << #x << " failed: " << SDL_GetError() << "\n("<<__FILE__<<", " << __LINE__ << ")";action}
#define CALL_SDL(x) __CALL_SDL(x,)
#define CALL_SDL_CRITICAL(x) __CALL_SDL(x,abort();)

#define PRINT_VALUE(x) qDebug() << "Value of `" << #x << "` = " << x;

#define TWEAKBAR_BLENDER_AXIS " axisy=-z axisz=y "

#endif // GLRT_DEPENDENCIES_H
