/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/gtpu/gtpu_teid.h"
#include "srsran/support/compiler.h"

namespace srsran {

/// \brief GTP-U TEID pool
///
/// This class provides a TEID pool that users can request unused TEIDs.
/// This allows the CU-UP/DU to allocate unused local TEIDs safely even
/// in the event of TEID wrap-around.
class gtpu_teid_pool
{
public:
  virtual ~gtpu_teid_pool() = default;

  [[nodiscard]] virtual expected<gtpu_teid_t> request_teid() = 0;

  [[nodiscard]] virtual bool release_teid(gtpu_teid_t teid) = 0;

  [[nodiscard]] virtual bool full() const = 0;

  virtual uint32_t get_max_nof_teids() = 0;
};
} // namespace srsran
