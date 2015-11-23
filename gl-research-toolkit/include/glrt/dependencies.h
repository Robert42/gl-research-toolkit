#ifndef GLRT_DEPENDENCIES_H
#define GLRT_DEPENDENCIES_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <SFML/Graphics.hpp>

#include <QDebug>

#include <iostream>


namespace glrt {

class Dependencies
{
public:
  Dependencies();
};

} // namespace glrt

#define PRINT_VALUE(x) qDebug() << "Value of `" << #x << "` = " << x;

#endif // GLRT_DEPENDENCIES_H
