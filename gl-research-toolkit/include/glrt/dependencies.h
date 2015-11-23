#ifndef GLRF_DEPENDENCIES_H
#define GLRF_DEPENDENCIES_H

#include <SFML/Graphics.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Window.hpp>
#include <SFGUI/Desktop.hpp>

#include <QDebug>

#include <iostream>

namespace glrf {

class Dependencies
{
public:
  Dependencies();
};

} // namespace glrf

#define PRINT_VALUE(x) qDebug() << "Value of `" << #x << "` = " << x;

#endif // GLRF_DEPENDENCIES_H
