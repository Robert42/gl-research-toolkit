#ifndef GLRT_DEPENDENCIES_H
#define GLRT_DEPENDENCIES_H

#include <SFML/Graphics.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Window.hpp>
#include <SFGUI/Desktop.hpp>

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
