#include <glrt/gui/properties.h>

namespace glrt {
namespace gui {


sfg::ToggleButton::Ptr createPropertyWidget(bool* value, const sf::String& label, BoolStyle style)
{
  sfg::ToggleButton::Ptr toggleButton;

  switch(style)
  {
  case BoolStyle::CheckButton:
    toggleButton = sfg::CheckButton::Create(label);
    break;
  case BoolStyle::ToggleButton:
  default:
    toggleButton = sfg::ToggleButton::Create(label);
  }

  toggleButton->SetActive(*value);

  // Note: I don't provide a smart ptr to the toggleButton itself to the lambda to avoid cyclic references with smart pointers.
  toggleButton->GetSignal(toggleButton->OnToggle).Connect([value](){*value = !*value;});

  return toggleButton;
}


} // namespace gui
} // namespace glrt

