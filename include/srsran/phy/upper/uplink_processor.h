/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

/// \file
/// \brief Uplink processor interface.

#pragma once

#include "srsran/phy/upper/uplink_pdu_slot_repository.h"

namespace srsran {

class slot_point;
class uplink_slot_processor;
class uplink_slot_processor_pool;

/// Uplink processor interface.
class uplink_processor
{
public:
  /// Default destructor.
  virtual ~uplink_processor() = default;

  /// \brief Configures the uplink processor for a given slot and returns the PDU repository interface.
  ///
  /// If the uplink processor assigned to the given slot is not available, the unique PDU slot repository will be
  /// invalid.
  virtual unique_uplink_pdu_slot_repository get_pdu_slot_repository(slot_point slot) = 0;

  /// Gets the slot processing interface.
  virtual uplink_slot_processor& get_slot_processor() = 0;
};

/// Pool of uplink processors.
class uplink_processor_pool
{
public:
  /// Default destructor.
  virtual ~uplink_processor_pool() = default;

  /// Gets the uplink slot processor pool.
  virtual uplink_slot_processor_pool& get_slot_processor_pool() = 0;

  /// Gets the uplink slot PDU repository pool interface.
  virtual uplink_pdu_slot_repository_pool& get_slot_pdu_repository_pool() = 0;
};
} // namespace srsran
