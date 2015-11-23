#ifndef GLRT_GUI_PROPERTIES_H
#define GLRT_GUI_PROPERTIES_H

#include <SFGUI/ToggleButton.hpp>
#include <SFGUI/CheckButton.hpp>

namespace glrt {
namespace gui {


enum class BoolStyle
{
  CheckButton,
  ToggleButton
};


sfg::ToggleButton::Ptr createPropertyWidget(bool* value, const sf::String& label, BoolStyle style=BoolStyle::CheckButton);


} // namespace gui
} // namespace glrt

#endif // GLRT_GUI_PROPERTIES_H
