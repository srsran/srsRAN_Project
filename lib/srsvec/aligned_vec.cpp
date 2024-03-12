/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/srsvec/aligned_vec.h"
#include "simd.h"
#include "srsran/support/error_handling.h"
#include <cstdlib>

void* srsran::srsvec::detail::mem_alloc(std::size_t size)
{
  void* ptr = nullptr;
  int   ret = ::posix_memalign(&ptr, SIMD_BYTE_ALIGN * 8, size);
  report_fatal_error_if_not(ret == 0 && ptr, "Failed posix_memalign.");
  return ptr;
}

void srsran::srsvec::detail::mem_free(void* ptr)
{
  ::free(ptr);
}
