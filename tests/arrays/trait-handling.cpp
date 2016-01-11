#include <glrt/toolkit/array.h>

#include <testing-framework.h>

#include "statespy.h"

using glrt::Array;


void test_capacity_traits()
{
  int cache;
  typedef glrt::ArrayCapacityTraits_Capacity_Blocks<16, 64> capacity_blocks;

  EXPECT_EQ(capacity_blocks::new_capacity(0, 0, 0, &cache), 0);
  EXPECT_EQ(capacity_blocks::new_capacity(1024, 0, 0, &cache), 1024);
  EXPECT_EQ(capacity_blocks::new_capacity(0, 0, 1, &cache), 16);
  EXPECT_EQ(capacity_blocks::new_capacity(0, 1, 0, &cache), 16);
  EXPECT_EQ(capacity_blocks::new_capacity(0, 15, 1, &cache), 16);
  EXPECT_EQ(capacity_blocks::new_capacity(0, 15, 2, &cache), 32);
  EXPECT_EQ(capacity_blocks::new_capacity(0, 16, 1, &cache), 32);
  EXPECT_EQ(capacity_blocks::new_capacity(1024, 16, 1, &cache), 1024);

  EXPECT_EQ(capacity_blocks::adapt_capacity_after_removing_elements(32, 16, 1, &cache), 32);
  EXPECT_EQ(capacity_blocks::adapt_capacity_after_removing_elements(64, 16, 1, &cache), 64);
  EXPECT_EQ(capacity_blocks::adapt_capacity_after_removing_elements(1024, 16, 1, &cache), 64);
  EXPECT_EQ(capacity_blocks::adapt_capacity_after_removing_elements(1024, 100, 1, &cache), 128);
  EXPECT_EQ(capacity_blocks::adapt_capacity_after_removing_elements(100, 100, 1, &cache), 100);
  EXPECT_EQ(capacity_blocks::adapt_capacity_after_removing_elements(1024, 66, 1, &cache), 128);
  EXPECT_EQ(capacity_blocks::adapt_capacity_after_removing_elements(100, 66, 1, &cache), 100);
  EXPECT_EQ(capacity_blocks::adapt_capacity_after_removing_elements(100, 65, 1, &cache), 100);
  EXPECT_EQ(capacity_blocks::adapt_capacity_after_removing_elements(100, 64, 1, &cache), 64);
}

void test_capacity_trait_rangecheck()
{
  typedef glrt::ArrayCapacityTraits_Capacity_Blocks<16, 64> capacity_blocks;
  typedef glrt::ArrayTraits_Unordered_Toolkit<StateSpy, capacity_blocks> traits;

  EXPECT_TRUE(traits::ranges_overlap( 0, 5, 0, 5));

  EXPECT_TRUE(traits::ranges_overlap( 1, 2, 0, 5));
  EXPECT_TRUE(traits::ranges_overlap(-1, 1, 0, 5));
  EXPECT_FALSE(traits::ranges_overlap(-1, 0, 0, 5));
  EXPECT_FALSE(traits::ranges_overlap(-10, -1, 0, 5));
  EXPECT_TRUE(traits::ranges_overlap(4, 5, 0, 5));
  EXPECT_TRUE(traits::ranges_overlap(4, 6, 0, 5));
  EXPECT_TRUE(traits::ranges_overlap(4, 7, 0, 5));
  EXPECT_FALSE(traits::ranges_overlap(5, 7, 0, 5));
  EXPECT_FALSE(traits::ranges_overlap(60, 70, 0, 5));

  EXPECT_TRUE(traits::ranges_overlap(0, 5,  1, 2));
  EXPECT_TRUE(traits::ranges_overlap(0, 5, -1, 1));
  EXPECT_FALSE(traits::ranges_overlap(0, 5, -1, 0));
  EXPECT_FALSE(traits::ranges_overlap(0, 5, -10, -1));
  EXPECT_TRUE(traits::ranges_overlap(0, 5, 4, 5));
  EXPECT_TRUE(traits::ranges_overlap(0, 5, 4, 6));
  EXPECT_TRUE(traits::ranges_overlap(0, 5, 4, 7));
  EXPECT_FALSE(traits::ranges_overlap(0, 5, 5, 7));
  EXPECT_FALSE(traits::ranges_overlap(0, 5, 60, 70));
}

void test_swap_instances_mO()
{
  StateSpy::clearIndex();
  typedef glrt::ArrayCapacityTraits_Capacity_Blocks<16, 64> capacity_blocks;
  typedef glrt::ArrayTraits_Unordered_Toolkit<StateSpy, capacity_blocks> traits;

  StateSpy instances[4];
  StateSpy::clearLog();

  traits::swap_instances_mO(instances+0, instances+1, 1);
  EXPECT_EQ(StateSpy::log(),
            "0: move operator from 1\n");
  StateSpy::clearLog();

  traits::swap_instances_mO(instances+2, instances+0, 2);
  EXPECT_EQ(StateSpy::log(),
            "2: move operator from 0\n"
            "3: move operator from 1\n");
  StateSpy::clearLog();
}

void test_swap_single_instance_mO()
{
  StateSpy::clearIndex();
  typedef glrt::ArrayCapacityTraits_Capacity_Blocks<16, 64> capacity_blocks;
  typedef glrt::ArrayTraits_Unordered_Toolkit<StateSpy, capacity_blocks> traits;

  StateSpy instances[4];
  StateSpy::clearLog();

  traits::swap_single_instance_mO(instances+1, instances+0);
  EXPECT_EQ(StateSpy::log(),
            "0: move operator from 1\n");
  StateSpy::clearLog();
}

void test_call_instance_destructors_D()
{
  StateSpy::clearIndex();
  typedef glrt::ArrayCapacityTraits_Capacity_Blocks<16, 64> capacity_blocks;
  typedef glrt::ArrayTraits_Unordered_Toolkit<StateSpy, capacity_blocks> traits;

  StateSpy instances[4];
  StateSpy::clearLog();

  traits::call_instance_destructors_D(instances, 4);
  EXPECT_EQ(StateSpy::log(),
            "0: destructed\n"
            "1: destructed\n"
            "2: destructed\n"
            "3: destructed\n");
  StateSpy::clearLog();
}

void test_values_used_to_fill_gaps()
{
  typedef glrt::ArrayCapacityTraits_Capacity_Blocks<16, 64> capacity_blocks;
  typedef glrt::ArrayTraits_Unordered_Toolkit<StateSpy, capacity_blocks> traits;
  int first, count;

  traits::values_used_to_fill_gaps(&first, &count, 4, 0, 4);
  EXPECT_EQ(first, 4);
  EXPECT_EQ(count, 0);

  traits::values_used_to_fill_gaps(&first, &count, 4, 1, 3);
  EXPECT_EQ(first, 4);
  EXPECT_EQ(count, 0);

  traits::values_used_to_fill_gaps(&first, &count, 4, 2, 2);
  EXPECT_EQ(first, 4);
  EXPECT_EQ(count, 0);

  traits::values_used_to_fill_gaps(&first, &count, 4, 3, 1);
  EXPECT_EQ(first, 4);
  EXPECT_EQ(count, 0);

  traits::values_used_to_fill_gaps(&first, &count, 4, 4, 0);
  EXPECT_EQ(first, 4);
  EXPECT_EQ(count, 0);

  traits::values_used_to_fill_gaps(&first, &count, 4, 2, 1);
  EXPECT_EQ(first, 3);
  EXPECT_EQ(count, 1);

  traits::values_used_to_fill_gaps(&first, &count, 4, 1, 1);
  EXPECT_EQ(first, 3);
  EXPECT_EQ(count, 1);

  traits::values_used_to_fill_gaps(&first, &count, 4, 0, 1);
  EXPECT_EQ(first, 3);
  EXPECT_EQ(count, 1);

  traits::values_used_to_fill_gaps(&first, &count, 4, 0, 0);
  EXPECT_EQ(first, 4);
  EXPECT_EQ(count, 0);

  traits::values_used_to_fill_gaps(&first, &count, 4, 0, 3);
  EXPECT_EQ(first,3);
  EXPECT_EQ(count, 1);
}

void test_traits()
{
  test_capacity_traits();
  test_values_used_to_fill_gaps();
}
