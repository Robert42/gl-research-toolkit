#include <glrt/debugging/draw-cameras.h>

#include <glrt/scene/scene.h>
#include <glrt/scene/camera-parameter.h>
#include <glrt/toolkit/aligned-vector.h>

namespace glrt {
namespace debugging {


DrawCameras::DrawCameras()
{
}


DrawCameras::~DrawCameras()
{
  deinit();
}


void DrawCameras::draw(const scene::Scene* scene)
{
  aligned_vector<scene::CameraParameter> cameras(aligned_vector<scene::CameraParameter>::Alignment::UniformBufferOffsetAlignment);
  cameras.reserve(scene->sceneCameras().length());
  for(const scene::CameraParameter& camera : scene->sceneCameras())
    cameras.push_back(camera);

  if(nCameras < scene->sceneCameras().length())
  {
    deinit();

    DebugMesh::Painter painter;

    painter.addSphere(1.f, 16);

    nCameras = scene->sceneCameras().length();
    cameraParameter = new gl::Buffer(cameras.size_in_bytes(), gl::Buffer::UsageFlag::IMMUTABLE, cameras.data());

    debugMesh = new DebugMesh(painter.toMesh());
  }else
  {
    cameras.writeTo(cameraParameter->Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
    cameraParameter->Unmap();
  }
}


void DrawCameras::deinit()
{
  delete debugMesh;
  delete cameraParameter;
  debugMesh = nullptr;
  cameraParameter = nullptr;
  nCameras = 0;
}


} // namespace debugging
} // namespace glrt

