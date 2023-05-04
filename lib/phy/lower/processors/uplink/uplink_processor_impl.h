/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/adt/tensor.h"
#include "srsran/gateways/baseband/baseband_gateway_buffer.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_baseband.h"
#include "srsran/phy/lower/sampling_rate.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/slot_point.h"
#include "srsran/support/srsran_assert.h"
#include <memory>

namespace srsran {

/// Implements a software generic lower PHY uplink processor.
class lower_phy_uplink_processor_impl : public lower_phy_uplink_processor, private uplink_processor_baseband
{
public:
  /// Configuration parameters.
  struct configuration {
    /// Sector identifier.
    unsigned sector_id;
    /// Subcarrier spacing.
    subcarrier_spacing scs;
    /// Cyclic prefix configuration.
    cyclic_prefix cp;
    /// Baseband sampling rate.
    sampling_rate rate;
    /// Number of receive ports.
    unsigned nof_rx_ports;
    /// Initial slot index within the radio frame.
    unsigned initial_slot_index;
  };

  /// \brief Constructs a software generic lower PHY uplink processor that can process PRACH and PUxCH.
  /// \param[in] prach_proc_ PRACH processor.
  /// \param[in] puxch_proc_ PUxCH processor.
  /// \param[in] config      Uplink processor configuration.
  lower_phy_uplink_processor_impl(std::unique_ptr<prach_processor> prach_proc_,
                                  std::unique_ptr<puxch_processor> puxch_proc_,
                                  const configuration&             config);

  // See interface for documentation.
  void connect(uplink_processor_notifier& notifier,
               prach_processor_notifier&  prach_notifier,
               puxch_processor_notifier&  puxch_notifier) override;

  // See interface for documentation.
  prach_processor_request_handler& get_prach_request_handler() override;

  // See interface for documentation.
  puxch_processor_request_handler& get_puxch_request_handler() override;

  // See interface for documentation.
  uplink_processor_baseband& get_baseband() override;

private:
  // See interface for documentation.
  void process(const baseband_gateway_buffer& samples) override;

  /// Processes a new symbol.
  void process_new_symbol();

  /// Sector identifier.
  unsigned sector_id;
  /// Number of receive ports.
  unsigned nof_rx_ports;
  /// Number of symbols per slot.
  unsigned nof_symbols_per_slot;
  /// Number of symbols per subframe.
  unsigned nof_symbols_per_subframe;
  /// Current number of buffered samples.
  unsigned current_nof_samples;
  /// Current symbol index within the slot.
  unsigned current_symbol_index;
  /// Current slot point.
  slot_point current_slot;
  /// List of the symbol sizes in number samples for each symbol within the subframe.
  std::vector<unsigned> symbol_sizes;
  /// Temporal storage of baseband samples.
  baseband_gateway_buffer_dynamic temp_buffer;
  /// Internal PRACH processor.
  std::unique_ptr<prach_processor> prach_proc;
  /// Internal PUxCH processor.
  std::unique_ptr<puxch_processor> puxch_proc;
  /// Uplink processor notifier.
  uplink_processor_notifier* notifier = nullptr;
};

} // namespace srsran
