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

#include "srsran/ran/slot_point.h"

namespace srsran {

struct lower_phy_rx_symbol_context;
struct resource_grid_context;
class resource_grid_reader;

/// \brief Lower physical layer PDxCH processor - Notifier interface.
///
/// Notifies a variety of PDxCH-related events such as requests outside the occasion window or overflows of PDxCH
/// requests.
class pdxch_processor_notifier
{
public:
  /// Default destructor.
  virtual ~pdxch_processor_notifier() = default;

  /// \brief Notifies the unavailability of a downlink resource grid.
  ///
  /// This error happens in a sector when the resource grid for the processing slot is not available at the time when a
  /// symbol is modulated.
  ///
  /// The time window the lower physical layer can receive a resource grid for a slot starts with
  /// lower_phy_timing_notifier::on_tti_boundary() and finishes with the beginning of the processing of the first symbol
  /// within the slot.
  ///
  /// \param[in] context Context in which the resource grid is not available.
  /// \sa lower_phy_rg_handler::handle_resource_grid.
  virtual void on_pdxch_request_late(const resource_grid_context& context) = 0;
};

} // namespace srsran
