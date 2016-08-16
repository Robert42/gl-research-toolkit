#include <glrt/renderer/voxel-buffer.h>

namespace glrt {
namespace renderer {

// see https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-iii-tree-construction-gpu/
BVH::SubTree BVH::generateHierarchy(quint16 begin, quint16 end)
{
  SubTree root;

  Q_ASSERT(begin!=end);
  if(begin+1 == end)
  {
    root.index = 0x8000 | begin;
    root.bounding_sphere = leaves_bounding_spheres[begin];
  }else
  {
    quint16 newNode = addInnerNode();
    quint16 split = findSplit(begin, end);

    Q_ASSERT(split != begin);
    Q_ASSERT(split != end);

    SubTree left_subtree = generateHierarchy(begin, split);
    SubTree right_subtree = generateHierarchy(split, end);

    bvhInnerNodes[newNode].left_child = left_subtree.index;
    bvhInnerNodes[newNode].right_child = right_subtree.index;

    root.index = newNode;
    root.bounding_sphere = left_subtree.bounding_sphere | right_subtree.bounding_sphere;
  }

  return root;
}

inline quint32 BVH::zIndexDistance(quint16 a, quint16 b)
{
  Q_ASSERT(a<num_leaves);
  Q_ASSERT(b<num_leaves);
  return glm::highestBitValue(leaves_z_indices[a] ^ leaves_z_indices[b]);
}

// see also https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-iii-tree-construction-gpu/
quint16 BVH::findSplit(quint16 begin, quint16 end)
{
  quint16 min_split_value=begin+1;
  quint16 max_split_value=end-1;

  quint16 split = (begin+end)/2;

  while(begin+1 < end)
  {
    Q_ASSERT(split>0);
    Q_ASSERT(end>0);

    if(zIndexDistance(begin, split-1) > zIndexDistance(split, end-1))
      end = split;
    else
      begin = split;

    split = (begin+end)/2;
  }

  return glm::clamp<quint16>(split, min_split_value, max_split_value);
}

} // namespace renderer
} // namespace glrt
