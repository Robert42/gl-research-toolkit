#include <glrt/toolkit/array.h>

#include <testing-framework.h>

void test_add_remove()
{
}


int main(int argc, char** argv)
{
  TestingApplication testing_application(argc, argv);

  test_add_remove();

  return testing_application.result();
}
