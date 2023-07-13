/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/gtpu/gtpu_teid.h"
#include "srsran/support/compiler.h"
#include <vector>

namespace srsran {

class gtpu_teid_pool
{
public:
  virtual ~gtpu_teid_pool() = default;

  SRSRAN_NODISCARD virtual bool allocate(gtpu_teid_t& teid) = 0;

  SRSRAN_NODISCARD virtual bool free(gtpu_teid_t teid) = 0;

  virtual bool full() const = 0;

  virtual uint32_t get_max_teids() = 0;
};
} // namespace srsran
