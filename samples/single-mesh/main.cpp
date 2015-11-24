#include <glrt/application.h>
#include <glrt/gui/toolbar.h>

#include <glhelper/gl.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::simpleWindow("Single Mesh"));

  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(GLRT_ASSET_DIR"/common/meshes/suzanne/suzanne.obj",
                                           // aiProcess_RemoveComponent |  TODO: use this
                                           aiProcess_JoinIdenticalVertices | // TODO: this can be used to allow switching between indexed and not indexed mode
                                           aiProcess_PreTransformVertices | // TODO: have this only activated when loading a single mesh
                                           aiProcess_CalcTangentSpace |
                                           aiProcess_GenNormals |
                                           aiProcess_ValidateDataStructure |
                                           aiProcess_RemoveRedundantMaterials |
                                           aiProcess_OptimizeMeshes |
                                           aiProcess_OptimizeGraph |
                                           aiProcess_ImproveCacheLocality |
                                           aiProcess_SortByPType  | // TODO Read the documentation to this
                                           aiProcess_FindDegenerates  |
                                           aiProcess_FindInvalidData  |
                                           aiProcess_GenUVCoords  | // TODO: think about this
                                           aiProcess_Triangulate
                                           );
  if(!scene)
  {
    qCritical() << "Couldn't load mesh: " << importer.GetErrorString();
    exit(-1);
  }

  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
    }

    app.update();

    GL_CALL(glClear, GL_COLOR_BUFFER_BIT);

    // T O D O : Add your drawing code here

    app.swapWindow();
  }

  return 0;
}
