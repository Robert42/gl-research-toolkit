#include <testing-framework.h>

void test_trait_handling();

int main(int argc, char** argv)
{
  TestingApplication testing_application(argc, argv);

  test_trait_handling();

  return testing_application.result();
}
