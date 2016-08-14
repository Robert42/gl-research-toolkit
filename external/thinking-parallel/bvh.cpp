#include <glrt/renderer/voxel-buffer.h>

namespace glrt {
namespace renderer {

// see https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-iii-tree-construction-gpu/
quint16 BVH::generateHierarchy(quint16 begin, quint16 end)
{
  quint16 newNode = addInnerNode();
  InnerNode innerNode;
  BoundingSphere boundingSphere;

  Q_ASSERT(begin!=end);
  if(begin+1 == end)
  {
    innerNode.leftChild = begin;
    innerNode.rightChild = begin;
    boundingSphere = leaves_bounding_spheres[begin];
  }else
  {
    quint16 split = findSplit(begin, end);

    innerNode.leftChild = generateHierarchy(begin, split);
    innerNode.rightChild = generateHierarchy(split, end);
    boundingSphere = bvhInnerBoundingSpheres[innerNode.leftChild] | bvhInnerBoundingSpheres[innerNode.rightChild];
  }


  bvhInnerNodes[newNode] = innerNode;
  bvhInnerBoundingSpheres[newNode] = boundingSphere;

  return newNode;
}

inline quint32 BVH::zIndexDistance(quint16 a, quint16 b)
{
  return glm::highestBitValue(leaves_z_indices[a] ^ leaves_z_indices[b]);
}

// see also https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-iii-tree-construction-gpu/
quint16 BVH::findSplit(quint16 begin, quint16 end)
{
  quint16 split = (begin+end)/2;

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
