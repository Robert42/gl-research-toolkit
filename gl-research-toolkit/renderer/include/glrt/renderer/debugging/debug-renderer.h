#ifndef GLRT_RENDERER_DEBUGGING_DEBUGRENDERER_H
#define GLRT_RENDERER_DEBUGGING_DEBUGRENDERER_H

#include <glrt/gui/anttweakbar.h>
#include <QObject>

namespace glrt {
namespace renderer {
namespace debugging {

class DebugRenderer final : public QObject
{
  Q_OBJECT
public:
  class Implementation;
  class List;

  typedef std::function<Implementation*()> ImplementationFactory;

  gui::TweakBarCBVar<bool> guiToggle;

  // scene is allowed to be nullptr. If scene is not nullptr, reinit is called after a new scene was loaded
  DebugRenderer(scene::Scene* scene, const ImplementationFactory& factory);
  DebugRenderer(const ImplementationFactory& factory);

  DebugRenderer(const DebugRenderer& other);
  ~DebugRenderer();

  DebugRenderer& operator=(const DebugRenderer&) = delete;
  DebugRenderer& operator=(DebugRenderer&&) = delete;

  void setEnabled(bool enabled);
  bool isEnabled() const;

  scene::Scene* scene() const;

public slots:
  void reinit();

signals:
  void debuggingRenderingEnabled(DebugRenderer* renderer, Implementation* implementation);
  void debuggingRenderingDisabled(DebugRenderer* renderer, Implementation* implementation);

private:
  scene::Scene* _scene = nullptr;
  ImplementationFactory _factory;

  Implementation* _implementation = nullptr;
  bool _enabled = false;

  QMetaObject::Connection _connection;

  void createImplementation();
  void deleteImplementation();
};


class DebugRenderer::Implementation : public QObject
{
  Q_OBJECT
public:
  Implementation(const Implementation&&) = delete;
  void operator=(const Implementation&&) = delete;

  Implementation(const Implementation&) = delete;
  void operator=(const Implementation&) = delete;

  virtual void render() = 0;

protected:
  Implementation(){}
};

class DebugRenderer::List final : public QObject
{
  Q_OBJECT
public:
  QVector<Implementation*> implementations;

  void connectTo(DebugRenderer* renderer);

  void render();

private slots:
  void debuggingRenderingEnabled(DebugRenderer* renderer, Implementation* implementation);
  void debuggingRenderingDisabled(DebugRenderer* renderer, Implementation* implementation);

private:
  QMap<DebugRenderer*, Implementation*> order;
};

} // namespace debugging
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_DEBUGGING_DEBUGRENDERER_H
