#ifndef GLRT_RENDERER_SHADERSTORAGEFORMAT_H
#define GLRT_RENDERER_SHADERSTORAGEFORMAT_H


#include <glrt/scene/node.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/collect-scene-data.h>


#include <glhelper/buffer.hpp>


namespace glrt {
namespace renderer {


template<typename ElementType>
class VariadicElementBuffer
{
public:
  typedef VariadicElementBuffer<ElementType> this_type;

  static_assert(sizeof(ElementType) % (sizeof(float)*4) == 0, "ElementType have the size of a multiple of sizeof(float)*4");

  VariadicElementBuffer(const VariadicElementBuffer&) = delete;
  VariadicElementBuffer(VariadicElementBuffer&&) = delete;
  VariadicElementBuffer& operator=(const VariadicElementBuffer&) = delete;
  VariadicElementBuffer& operator=(VariadicElementBuffer&&) = delete;

  VariadicElementBuffer(int capacityIncrement = 16)
    : capacityIncrement(glm::max(1, capacityIncrement))
  {
    resize(0, 0);
  }

  ~VariadicElementBuffer()
  {
    deinit();
  }

  void bindShaderStorageBuffer(int bindingIndex)
  {
    buffer->BindShaderStorageBuffer(bindingIndex);
  }

  void reinit()
  {
    deinit();

    resize(0, 0);
  }

  void resize(int numberStaticElements, int numberMovableElements)
  {
    // static elements can't be removed
    Q_ASSERT(numberStaticElements >= this->numberStaticElements);

    // round up to a multiple of capacityIncrement
    int newCapacity = (numberStaticElements+numberMovableElements+capacityIncrement-1) / capacityIncrement;

    int newBufferSize = newCapacity*sizeof(ElementType) + sizeof(HeaderType);

    bool dirtyHeader = false;

    if(!buffer || newBufferSize > buffer->GetSize())
    {
      delete buffer;
      buffer = new gl::Buffer(newBufferSize, gl::Buffer::MAP_WRITE, nullptr);
      dirtyHeader = true;
    }

    int n = this->numberStaticElements + this->numberMovableElements;

    this->numberStaticElements = numberStaticElements;
    this->numberMovableElements = numberMovableElements;

    dirtyHeader |= n != this->numberStaticElements + this->numberMovableElements;

    if(dirtyHeader)
    {
      HeaderType& headerType = *reinterpret_cast<HeaderType*>(buffer->Map(0, sizeof(HeaderType), gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_RANGE));

      headerType.numElements = this->numberStaticElements + this->numberMovableElements;

      buffer->Unmap();
    }
  }

  void initStaticElements(int staticElementsToInit, const std::function<void(ElementType*,int)>& initElement)
  {
    if(staticElementsToInit == 0)
      return;

    Q_ASSERT(staticElementsToInit <= numberStaticElements);

    ElementType* element = mapElements(numberStaticElements-staticElementsToInit, staticElementsToInit);

    for(int i=0; i<staticElementsToInit; ++i)
      initElement(element+i, i);

    buffer->Unmap();
  }

  void updateMovableElements(const std::function<void(ElementType*,int)>& updateElement)
  {
    if(numberMovableElements==0)
      return;

    ElementType* element = mapElements(numberStaticElements, numberMovableElements);

    for(int i=0; i<numberMovableElements; ++i)
      updateElement(element+i, i);

    buffer->Unmap();
  }

private:
  struct HeaderType
  {
    int numElements;
    padding<int, 3> _padding;
  };

  static_assert(sizeof(HeaderType) % (sizeof(float)*4) == 0, "HeaderType have the size of a multiple of sizeof(float)*4");

  const int capacityIncrement;
  gl::Buffer* buffer = nullptr;
  int numberStaticElements = 0;
  int numberMovableElements = 0;

  ElementType* mapElements(int begin, int n)
  {
    Q_ASSERT(n>0);
    Q_ASSERT(sizeof(HeaderType)+begin*sizeof(ElementType)+n*sizeof(ElementType) <= buffer->GetSize());

    return reinterpret_cast<ElementType*>(buffer->Map(sizeof(HeaderType)+begin*sizeof(ElementType), n*sizeof(ElementType), gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_RANGE));
  }

  void deinit()
  {
    delete buffer;
    buffer = nullptr;
    numberStaticElements = 0;
    numberMovableElements = 0;
  }
};


template<class NodeComponentType, typename ElementType = typename NodeComponentType::Data>
class ShaderStorageFormat
{
public:
  typedef ShaderStorageFormat<NodeComponentType, ElementType> this_type;

  static_assert(std::is_base_of<scene::Node::Component, NodeComponentType>::value, "NodeComponentType must inherit Node::Component");

  // important: the given scene instance must exist longer than this shader-storage instance
  ShaderStorageFormat(scene::Scene& scene, int capacityIncrement = 16)
    : scene(scene),
      variadicElementBuffer(capacityIncrement)
  {
    sceneLoadedConnection = QObject::connect(&scene, &scene::Scene::sceneLoaded, std::bind(&this_type::handleLoadedScene, this, std::placeholders::_1));
    reinit();
  }

  ~ShaderStorageFormat()
  {
    deinit();
  }

  ShaderStorageFormat(const ShaderStorageFormat&) = delete;
  ShaderStorageFormat(ShaderStorageFormat&&) = delete;
  ShaderStorageFormat& operator=(const ShaderStorageFormat&) = delete;
  ShaderStorageFormat& operator=(ShaderStorageFormat&&) = delete;

  void reinit()
  {
    deinit();

    addComponents(glrt::scene::collectAllComponentsWithType<NodeComponentType>(&scene));
  }

  void addComponents(const QVector<NodeComponentType*>& components)
  {
    QVector<NodeComponentType*> newStaticComponents;

    for(NodeComponentType* component : components)
    {
      if(component->movable())
      {
        // prevent adding the same component multiple times
        if(connections.contains(component))
          continue;

        connections[component] = QObject::connect(component, &QObject::destroyed, std::bind(&this_type::handleDeletedComponent, this, std::placeholders::_1));
        movableComponents.append(component);
      }else
      {
        if(staticComponents.contains(component))
           continue;

        staticComponents.insert(component);

        newStaticComponents.append(component);
      }
    }

    variadicElementBuffer.resize(staticComponents.size(), movableComponents.length());

    if(!newStaticComponents.isEmpty())
    {
      variadicElementBuffer.initStaticElements(newStaticComponents.length(),
                                              [&newStaticComponents](ElementType* element, int i) {
        NodeComponentType* component = newStaticComponents[i];
        *element = component->globalCoordFrame() * component->data;
      });
    }
  }

  void removeComponent(NodeComponentType* component)
  {
    if(connections.contains(component))
    {
      QObject::disconnect(connections[component]);
      connections.remove(component);
      movableComponents.remove(movableComponents.indexOf(component));

      variadicElementBuffer.resize(staticComponents.size(), movableComponents.length());
    }
  }

  void update()
  {
    variadicElementBuffer.updateMovableElements([this](ElementType* element, int i) {
      NodeComponentType* component = movableComponents[i];
      *element = component->globalCoordFrame() * component->data;
    });
  }

  void bindShaderStorageBuffer(int bindingIndex)
  {
    variadicElementBuffer.bindShaderStorageBuffer(bindingIndex);
  }

private:
  scene::Scene& scene;
  QMetaObject::Connection sceneLoadedConnection;

  VariadicElementBuffer<ElementType> variadicElementBuffer;
  QHash<NodeComponentType*, QMetaObject::Connection> connections;
  QVector<NodeComponentType*> movableComponents;
  QSet<NodeComponentType*> staticComponents;


  void deinit()
  {
    variadicElementBuffer.reinit();
    staticComponents.clear();
    movableComponents.clear();

    for(const QMetaObject::Connection& connection : connections)
      QObject::disconnect(connection);
    connections.clear();
  }

  void handleLoadedScene(bool success)
  {
    if(success)
      reinit();
    else
      deinit();
  }

  void handleDeletedComponent(QObject* object)
  {
    NodeComponentType* component = reinterpret_cast<NodeComponentType*>(object);
    removeComponent(component);
  }
};


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SHADERSTORAGEFORMAT_H
