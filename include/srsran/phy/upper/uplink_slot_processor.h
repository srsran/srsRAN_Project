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

#include "srsran/phy/upper/channel_processors/prach_detector.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration.h"
#include "srsran/phy/upper/uplink_pdu_slot_repository.h"

namespace srsran {

class prach_buffer;
struct prach_buffer_context;
class slot_point;
class upper_phy_rx_results_notifier;
struct srs_estimator_configuration;
struct resource_grid_context;
class shared_resource_grid;
class uplink_pdu_slot_repository;

/// \brief Uplink slot processor.
///
/// Processes received symbols and PRACH.
class uplink_slot_processor
{
public:
  /// Default destructor.
  virtual ~uplink_slot_processor() = default;

  /// \brief Notify the reception of an OFDM symbol.
  ///
  /// \param[in] end_symbol_index Last received symbol in the slot.
  virtual void handle_rx_symbol(unsigned end_symbol_index, bool is_valid) = 0;

  /// \brief Processes the PRACH using the given configuration and context.
  ///
  /// The PRACH detection results will be notified by the upper_phy_rx_results_notifier with event \ref
  /// upper_phy_rx_results_notifier::on_new_prach_results "on_new_prach_results".
  ///
  /// \param[in] buffer   Channel symbols the PRACH detection is performed on.
  /// \param[in] context  Context used by the underlying PRACH detector.
  virtual void process_prach(const prach_buffer& buffer, const prach_buffer_context& context) = 0;

  /// Discards the slot processing due to an error.
  virtual void discard_slot() = 0;
};

/// \brief Pool of uplink processors.
///
/// This interface manages the access to the available uplink processors.
class uplink_slot_processor_pool
{
public:
  /// Default destructor.
  virtual ~uplink_slot_processor_pool() = default;

  /// \brief Returns an uplink slot processor for the given slot.
  ///
  /// Slots must be requested in order to detects gaps, intermediate slots that are not requested will be considered
  /// lost and the uplink processors will be released accordingly.
  ///
  /// \param slot[in]      Slot point.
  /// \return An uplink processor.
  virtual uplink_slot_processor& get_slot_processor(slot_point slot) = 0;
};
} // namespace srsran
