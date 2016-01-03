#include <glrt/dependencies.h>
#include <QApplication>

#include <angelscript-integration/call-script.h>

#include <sdk/add_on/scriptstdstring/scriptstdstring.h>
#include <sdk/add_on/scriptarray/scriptarray.h>

using AngelScript::asDWORD;
using AngelScriptIntegration::AngelScriptCheck;
using AngelScriptIntegration::RefCountedObject;
using AngelScriptIntegration::ref;
using AngelScriptIntegration::weakref;

const asDWORD ACESS_MASK_GRAPH = glrt::ACCESS_MASK_USER;

