#include <testing-framework.h>

void main_qt_tests();
void main_coord_frame();
void main_array_tests();
void test_shader_includes();
void main_shader_library();

int main(int argc, char** argv)
{
  TestingApplication testing_application(argc, argv);

  main_qt_tests();
  main_coord_frame();
  main_array_tests();
  test_shader_includes();
  main_shader_library();

  return testing_application.result();
}
