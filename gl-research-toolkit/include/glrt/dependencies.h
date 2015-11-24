#ifndef GLRT_DEPENDENCIES_H
#define GLRT_DEPENDENCIES_H

#include <GL/glew.h>

#include <SDL2/SDL.h>

#include <glm/glm.hpp>

#include <AntTweakBar.h>

#include <QDebug>
#include <QString>

#include <iostream>


namespace glrt {

class Dependencies final
{
public:
  Dependencies();
};

} // namespace glrt

#define __CALL_SDL(x, action) if(!(x)){qCritical() << #x << " failed: " << SDL_GetError() << "\n("<<__FILE__<<", " << __LINE__ << ")";action}
#define CALL_SDL(x) __CALL_SDL(x,)
#define CALL_SDL_CRITICAL(x) __CALL_SDL(x,exit(-1);)

#define PRINT_VALUE(x) qDebug() << "Value of `" << #x << "` = " << x;

#endif // GLRT_DEPENDENCIES_H
