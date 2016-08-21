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
    bvhInnerBoundingSpheres[newNode] = left_subtree.bounding_sphere | right_subtree.bounding_sphere;

    root.index = newNode;
    root.bounding_sphere = bvhInnerBoundingSpheres[newNode];
  }

  return root;
}

BVH::SubTree BVH::generateSingleElementHierarchy()
{
  SubTree root;

  quint16 newNode = addInnerNode();
  SubTree left_subtree = generateHierarchy(0, 1);
  SubTree right_subtree;
  right_subtree.index = 0x8001;
  right_subtree.bounding_sphere = BoundingSphere{glm::vec3(NAN), NAN};

  bvhInnerNodes[newNode].left_child = left_subtree.index;
  bvhInnerNodes[newNode].right_child = right_subtree.index;
  bvhInnerBoundingSpheres[newNode] = left_subtree.bounding_sphere;

  root.index = newNode;
  root.bounding_sphere = bvhInnerBoundingSpheres[newNode];
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
#define FORCE_BALANCED 0
#define PRINT_SPLITS 0

#if PRINT_SPLITS
  qDebug() << "\n\n\nBVH::findSplit("<<begin<<"," << end << ")";
  QString str1;
  for(quint16 i=begin; i<end; ++i)
    str1 += QString("0x%0 ").arg(QString::number(leaves_z_indices[i], 16));
  qDebug() << "  before split:\n"<<str1.toStdString().c_str();
#endif

  quint16 split = (begin+end)/2;

#if !FORCE_BALANCED
  quint16 range_begin=begin;
  quint16 range_end=end;

  while(range_begin+1 < range_end)
  {
    Q_ASSERT(split>0);
    Q_ASSERT(range_end>0);

    if(zIndexDistance(range_begin, split-1) > zIndexDistance(split, range_end-1))
      range_end = split;
    else
      range_begin = split;

    split = (range_begin+range_end)/2;
  }
#endif

  split = glm::clamp<quint16>(split, begin+1, end-1);

#if PRINT_SPLITS
  QString str2;
  for(quint16 i=begin; i<split; ++i)
    str1 += QString("0x%0 ").arg(QString::number(leaves_z_indices[i], 16));
  for(quint16 i=split; i<end; ++i)
    str2 += QString("0x%0 ").arg(QString::number(leaves_z_indices[i], 16));
  qDebug() << "  after split:\n"<<str1.toStdString().c_str()<<"  ||  "<<str2.toStdString().c_str();
#endif

  return split;
}

} // namespace renderer
} // namespace glrt
