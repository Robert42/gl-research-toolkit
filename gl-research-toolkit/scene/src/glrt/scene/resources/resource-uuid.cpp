#include <glrt/scene/resources/resource-uuid.h>
#include <angelscript-integration/angelscript-integration.h>

#include <QRegularExpression>

namespace glrt {
namespace scene {
namespace resources {


using AngelScriptIntegration::AngelScriptCheck;


inline QUuid cast_custom_uuid_type(const QUuid& src)
{
  return src;
}

#define UUID_CHARS(n) "[a-fA-F0-9]{"#n"}"

inline void init_custom_uuid_type(QUuid* in, const std::string& s)
{
  QString str = QString::fromStdString(s);

  static QRegularExpression regex("^\\{" UUID_CHARS(8) "-" UUID_CHARS(4) "-" UUID_CHARS(4) "-" UUID_CHARS(4) "-" UUID_CHARS(12) "\\}$");

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

