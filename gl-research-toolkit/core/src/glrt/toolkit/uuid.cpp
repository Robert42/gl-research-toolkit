#include <glrt/toolkit/uuid.h>
#include <angelscript-integration/angelscript-integration.h>

#include <QRegularExpression>


namespace glrt {

using AngelScriptIntegration::AngelScriptCheck;

// ======== UuidIndex ==========================================================

std::mutex& UuidIndex::mutex()
{
  static std::mutex m;
  return m;
}

std::unordered_map<QUuid, UuidIndex::index_type>& UuidIndex::allIndicesForUuid()
{
  static std::unordered_map<QUuid, UuidIndex::index_type> m;
  return m;
}

std::vector<QUuid>& UuidIndex::allUuidsForIndex()
{
  static std::vector<QUuid> v;
  return v;
}

UuidIndex::UuidIndex(const QUuid &uuid)
  : index(indexForUuid(uuid))
{
}

QUuid UuidIndex::toQUuid() const
{
  return uuidForIndex(index);
}

bool UuidIndex::operator==(UuidIndex other) const
{
  return other.index == this->index;
}

bool UuidIndex::operator!=(UuidIndex other) const
{
  return other.index != this->index;
}

bool UuidIndex::operator<(UuidIndex other) const
{
  return other.index < this->index;
}

bool UuidIndex::operator<=(UuidIndex other) const
{
  return other.index <= this->index;
}

bool UuidIndex::operator>(UuidIndex other) const
{
  return other.index > this->index;
}

bool UuidIndex::operator>=(UuidIndex other) const
{
  return other.index >= this->index;
}

QUuid UuidIndex::uuidForIndex(index_type i)
{
  std::lock_guard<std::mutex> lock(mutex());
  Q_UNUSED(lock);

  if(i >= allUuidsForIndex().size())
    throw GLRT_EXCEPTION("Trying to access unregeistered uuid");

  return allUuidsForIndex().at(i);
}

UuidIndex::index_type UuidIndex::indexForUuid(const QUuid& uuid)
{
  std::lock_guard<std::mutex> lock(mutex());
  Q_UNUSED(lock);

  auto i = allIndicesForUuid().find(uuid);

  if(i!=allIndicesForUuid().end())
    return i->second;

  index_type index = allUuidsForIndex().size();

  allUuidsForIndex().push_back(uuid);
  allIndicesForUuid().insert(std::make_pair(uuid, index));

  return index;
}

UuidIndex::UuidIndex(index_type index)
  : index(index)
{
}

// ======== Uuid & AngelScript =================================================


inline Uuid<void> cast_custom_uuid_type(const Uuid<void>& src)
{
  return src;
}


inline void init_custom_uuid_type(Uuid<void>* in, const std::string& s)
{
#define UUID_CHARS(n) "[a-fA-F0-9]{"#n"}"
  static QRegularExpression regex("^\\{" UUID_CHARS(8) "-" UUID_CHARS(4) "-" UUID_CHARS(4) "-" UUID_CHARS(4) "-" UUID_CHARS(12) "\\}$");
#undef UUID_CHARS

  QString str = QString::fromStdString(s);

  if(regex.match(str).hasMatch())
  {
    *in = Uuid<void>(QUuid(str));
  }else
  {
    *in = Uuid<void>(QUuid::createUuidV5(QUuid(), str));
    qWarning() << "Invalid uuid in script " << str << " replaced with " << *in;
  }
}


inline void deinit_custom_uuid_type(Uuid<void>*)
{
}


inline std::string cast_custom_uuid_to_string(Uuid<void>* in)
{
  return in->toQUuid().toString().toStdString();
}


inline void RegisterUuidType(const char* name)
{
  int r;
  r = angelScriptEngine->RegisterObjectType(name,
                                            sizeof(Uuid<void>),
                                            AngelScript::asOBJ_VALUE |
                                            AngelScript::asOBJ_POD |
                                            AngelScript::asOBJ_APP_CLASS_CDAK);
  AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectBehaviour(name, AngelScript::asBEHAVE_CONSTRUCT, "void ctor(string &in)", AngelScript::asFUNCTION(init_custom_uuid_type), AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectMethod(name, "string opConv()", AngelScript::asFUNCTION(cast_custom_uuid_to_string), AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptCheck(r);
}


template<>
void Uuid<void>::registerCustomizedUuidType(const char* innerType, bool declareInnerType)
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);
  int r;

  std::string name = "Uuid<"+std::string(innerType)+">";
  const char* name_cstr = name.c_str();

  if(declareInnerType)
  {
    r = angelScriptEngine->RegisterObjectType(innerType, 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT);
    AngelScriptCheck(r);
  }

  RegisterUuidType(name_cstr);

  r = angelScriptEngine->RegisterObjectMethod(name_cstr, "BaseUuid opImplConv()", AngelScript::asFUNCTION(cast_custom_uuid_type), AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("BaseUuid", (name+" opConv()").c_str(), AngelScript::asFUNCTION(cast_custom_uuid_type), AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptCheck(r);
  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


template<>
void Uuid<void>::registerAngelScriptAPI()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  RegisterUuidType("BaseUuid");

  int r;
  r = angelScriptEngine->RegisterObjectType("Uuid<class T>",
                                            sizeof(Uuid<void>),
                                            AngelScript::asOBJ_VALUE |
                                            AngelScript::asOBJ_TEMPLATE |
                                            AngelScript::asGetTypeTraits<Uuid<void>>());
  AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour("Uuid<T>", AngelScript::asBEHAVE_CONSTRUCT, "void ctor(const string &in)", AngelScript::asFUNCTION(init_custom_uuid_type), AngelScript::asCALL_CDECL_OBJFIRST);
  r = angelScriptEngine->RegisterObjectBehaviour("Uuid<T>", AngelScript::asBEHAVE_DESTRUCT, "void dtor()", AngelScript::asFUNCTION(deinit_custom_uuid_type), AngelScript::asCALL_CDECL_OBJFIRST);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace glrt

