#ifndef GLRT_WINDOW_H
#define GLRT_WINDOW_H

#include <SFML/Graphics.hpp>

namespace glrt {

class Window
{
public:
  sf::RenderWindow window;

  Window(int argc, char** argv);
};

} // namespace glrt

#endif // GLRT_WINDOW_H
