/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/srsvec/aligned_vec.h"
#include "simd.h"
#include "srsgnb/support/srsran_assert.h"
#include <cstdlib>

void* srsgnb::srsvec::detail::mem_alloc(std::size_t size)
{
  void* ptr = nullptr;
  int   ret = posix_memalign(&ptr, SIMD_BYTE_ALIGN * 8, size);
  SRSGNB_ALWAYS_ASSERT__(ret == 0 && ptr, "Failed posix_memalign.");
  return ptr;
}

void srsgnb::srsvec::detail::mem_free(void* ptr)
{
  free(ptr);
}
