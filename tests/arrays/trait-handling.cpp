#include <glrt/toolkit/array.h>

#include <testing-framework.h>

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
