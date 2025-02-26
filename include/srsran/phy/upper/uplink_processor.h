/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  /// If the uplink processor assigned to the given slot is not available, the slot repository will contain an invalid.
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
  virtual uplink_pdu_slot_repository_pool& get_slot_pdu_repository() = 0;
};
} // namespace srsran
