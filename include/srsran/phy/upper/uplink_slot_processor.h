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
  /// \param[in] grid             Resource grid.
  /// \param[in] end_symbol_index Last received symbol in the slot.
  virtual void handle_rx_symbol(const shared_resource_grid& grid, unsigned end_symbol_index) = 0;

  /// \brief Processes the PRACH using the given configuration and context.
  ///
  /// The PRACH detection results will be notified by the upper_phy_rx_results_notifier with event \ref
  /// upper_phy_rx_results_notifier::on_new_prach_results "on_new_prach_results".
  ///
  /// \param[in] buffer   Channel symbols the PRACH detection is performed on.
  /// \param[in] context  Context used by the underlying PRACH detector.
  virtual void process_prach(const prach_buffer& buffer, const prach_buffer_context& context) = 0;
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
  /// \param slot[in]      Slot point.
  /// \return An uplink processor.
  virtual uplink_slot_processor& get_slot_processor(slot_point slot) = 0;
};
} // namespace srsran
