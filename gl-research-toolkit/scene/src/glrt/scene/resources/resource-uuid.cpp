#include <glrt/scene/resources/resource-uuid.h>
#include <angelscript-integration/angelscript-integration.h>

#include <QRegularExpression>

namespace glrt {
namespace scene {
namespace resources {

void pass_arg_to_angelscript(AngelScript::asIScriptContext*context, int i, QUuid* value)
{
  context->SetArgObject(i, value);
}

void pass_arg_to_angelscript(AngelScript::asIScriptContext*context, int i, StaticMeshUuid* value)
{
  context->SetArgObject(i, value);
}

void pass_arg_to_angelscript(AngelScript::asIScriptContext*context, int i, MaterialDataUuid* value)
{
  context->SetArgObject(i, value);
}

using AngelScriptIntegration::AngelScriptCheck;


inline QUuid cast_custom_uuid_type(const QUuid& src)
{
  return src;
}


inline void init_custom_uuid_type(QUuid* in, const std::string& s)
{
#define UUID_CHARS(n) "[a-fA-F0-9]{"#n"}"
  static QRegularExpression regex("^\\{" UUID_CHARS(8) "-" UUID_CHARS(4) "-" UUID_CHARS(4) "-" UUID_CHARS(4) "-" UUID_CHARS(12) "\\}$");
#undef UUID_CHARS

  QString str = QString::fromStdString(s);

  if(regex.match(str).hasMatch())
  {
    *in = QUuid(str);
  }else
  {
    *in = QUuid::createUuidV5(QUuid(), str);
    qWarning() << "Invalid uuid in script " << str << " replaced with " << *in;
  }
}


inline std::string cast_custom_uuid_to_string(QUuid* in)
{
  return in->toString().toStdString();
}


inline void RegisterUuidType(const char* name)
{
  int r;
  r = angelScriptEngine->RegisterObjectType(name,
                                            sizeof(QUuid),
                                            AngelScript::asOBJ_VALUE |
                                            AngelScript::asOBJ_APP_PRIMITIVE |
                                            AngelScript::asOBJ_POD);
  AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectBehaviour(name, AngelScript::asBEHAVE_CONSTRUCT, "void ctor(string &in)", AngelScript::asFUNCTION(init_custom_uuid_type), AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectMethod(name, "string opConv()", AngelScript::asFUNCTION(cast_custom_uuid_to_string), AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptCheck(r);
}


inline void RegisterCustomizedUuidType(const char* name)
{
  int r;

  RegisterUuidType(name);

  r = angelScriptEngine->RegisterObjectMethod(name, "Uuid opImplConv()", AngelScript::asFUNCTION(cast_custom_uuid_type), AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("Uuid", (std::string(name)+" opConv()").c_str(), AngelScript::asFUNCTION(cast_custom_uuid_type), AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptCheck(r);
}


template<>
void Uuid<void>::registerAngelScriptAPI()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  RegisterUuidType("Uuid");
  RegisterCustomizedUuidType("StaticMeshUuid");
  RegisterCustomizedUuidType("MaterialDataUuid");

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace resources
} // namespace scene
} // namespace glrt

