#include <glrt/renderer/voxel-buffer.h>

namespace glrt {
namespace renderer {

// see https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-iii-tree-construction-gpu/
int BVH::generateHierarchy(int begin, int end)
{
  int newNode = addInnerNode();
  InnerNode innerNode;
  BoundingSphere boundingSphere;

  Q_ASSERT(begin!=end);
  if(begin+1 == end)
  {
    innerNode.leftChild = begin;
    innerNode.rightChild = begin;
    boundingSphere = leaves[begin]->boundingSphere();
  }else
  {
    int split = findSplit(begin, end);

    innerNode.leftChild = generateHierarchy(begin, split);
    innerNode.rightChild = generateHierarchy(split, end);
    boundingSphere = bvhInnerBoundingSpheres[innerNode.leftChild] | bvhInnerBoundingSpheres[innerNode.rightChild];
  }


  bvhInnerNodes[newNode] = innerNode;
  bvhInnerBoundingSpheres[newNode] = boundingSphere;

  return newNode;
}

inline quint32 BVH::zIndexDistance(int a, int b)
{
  return glm::highestBitValue(zIndices[a] ^ zIndices[b]);
}

// see also https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-iii-tree-construction-gpu/
int BVH::findSplit(int begin, int end)
{
  int split = (begin+end)/2;

  while(begin+1 < end)
  {
    if(zIndexDistance(begin, split) > zIndexDistance(split, end))
      end = split;
    else
      begin = split;

    split = (begin+end)/2;
  }

  return split;
}

} // namespace renderer
} // namespace glrt
