#ifndef GLRF_DEPENDENCIES_H
#define GLRF_DEPENDENCIES_H

#include <SFML/Graphics.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Window.hpp>
#include <SFGUI/Desktop.hpp>

#include <iostream>

namespace glrf {

class Dependencies
{
public:
  Dependencies();
};

} // namespace glrf

#define PRINT_VALUE(x) std::cout << "Value of `" << #x << "` = " << x << std::endl;

#endif // GLRF_DEPENDENCIES_H
