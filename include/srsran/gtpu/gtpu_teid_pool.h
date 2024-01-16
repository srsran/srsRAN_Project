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

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/gtpu/gtpu_teid.h"
#include "srsran/support/compiler.h"
#include <vector>

namespace srsran {

/// \brief GTP-U TEID pool
///
/// This class provides a TEID pool that users can request unused TEIDs.
/// This allows the CU-UP/DU to allocate unused local TEIDs safely event
/// in the event of TEID wrap-around.
class gtpu_teid_pool
{
public:
  virtual ~gtpu_teid_pool() = default;

  SRSRAN_NODISCARD virtual expected<gtpu_teid_t> request_teid() = 0;

  SRSRAN_NODISCARD virtual bool release_teid(gtpu_teid_t teid) = 0;

  virtual bool full() const = 0;

  virtual uint32_t get_max_teids() = 0;
};
} // namespace srsran
