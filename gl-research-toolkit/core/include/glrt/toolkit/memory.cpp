#include "memory.h"

void swap_memory(void* a, void* b, size_t num_bytes_to_swap)
{
  byte* array_a = reinterpret_cast<byte*>(a);
  byte* array_b = reinterpret_cast<byte*>(b);

  for(size_t i=0; i<num_bytes_to_swap; ++i)
    std::swap(array_a[i], array_b[i]);
}
