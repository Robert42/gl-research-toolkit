#include "common.h"

void test_graph_with_accessors(AngelScript::asIScriptEngine* engine);

int main(int argc, char** argv)
{
  QApplication application(argc, argv);
  AngelScript::asIScriptEngine* engine = AngelScript::asCreateScriptEngine();
  engine->SetEngineProperty(AngelScript::asEP_REQUIRE_ENUM_SCOPE, 1);
  AngelScriptIntegration::init_message_callback_qt(engine);


  asDWORD previousMask = engine->SetDefaultAccessMask(AngelScriptIntegration::ACCESS_MASK_ALL);
  AngelScript::RegisterStdString(engine);
  AngelScript::RegisterScriptArray(engine, true);
  engine->SetDefaultAccessMask(previousMask);

  test_graph_with_accessors(engine);

  Q_UNUSED(application);
  return 0;
}
