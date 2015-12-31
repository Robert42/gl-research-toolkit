#include <glrt/toolkit/uuid.h>
#include <angelscript-integration/angelscript-integration.h>

#include <QRegularExpression>

namespace glrt {


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


inline void deinit_custom_uuid_type(QUuid*)
{
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


inline void RegisterCustomizedUuidType(const char* innerType, bool declareInnerType)
{
  int r;

  std::string name = "Uuid<"+std::string(innerType)+">";
  const char* name_cstr = name.c_str();

  if(declareInnerType)
  {
    r = angelScriptEngine->RegisterObjectType(innerType, 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT);
    AngelScriptCheck(r);
  }

  RegisterUuidType(name_cstr);

  r = angelScriptEngine->RegisterObjectMethod(name_cstr, "QUuid opImplConv()", AngelScript::asFUNCTION(cast_custom_uuid_type), AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("QUuid", (name+" opConv()").c_str(), AngelScript::asFUNCTION(cast_custom_uuid_type), AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptCheck(r);
}


template<>
void Uuid<void>::registerAngelScriptAPI()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  RegisterUuidType("QUuid");

  int r;
  r = angelScriptEngine->RegisterObjectType("Uuid<class T>",
                                            sizeof(QUuid),
                                            AngelScript::asOBJ_VALUE |
                                            AngelScript::asOBJ_TEMPLATE |
                                            AngelScript::asGetTypeTraits<QUuid>());
  AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour("Uuid<T>", AngelScript::asBEHAVE_CONSTRUCT, "void ctor(const string &in)", AngelScript::asFUNCTION(init_custom_uuid_type), AngelScript::asCALL_CDECL_OBJFIRST);
  r = angelScriptEngine->RegisterObjectBehaviour("Uuid<T>", AngelScript::asBEHAVE_DESTRUCT, "void dtor()", AngelScript::asFUNCTION(deinit_custom_uuid_type), AngelScript::asCALL_CDECL_OBJFIRST);

  RegisterCustomizedUuidType("StaticMeshData", true); // TODO: remove
  RegisterCustomizedUuidType("MaterialData", true); // TODO: remove

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace glrt

