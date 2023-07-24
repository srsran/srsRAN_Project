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
#include "srsran/adt/blocking_queue.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/phy/lower/amplitude_controller/amplitude_controller.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_baseband.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_notifier.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_baseband.h"
#include "srsran/phy/lower/sampling_rate.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/support/stats.h"

namespace srsran {

/// Collects downlink processor baseband configuration parameters.
struct downlink_processor_baseband_configuration {
  /// Sector identifier.
  unsigned sector_id;
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Cyclic prefix configuration.
  cyclic_prefix cp;
  /// Baseband sampling rate.
  sampling_rate rate;
  /// Number of transmit ports.
  unsigned nof_tx_ports;
  /// Number of slots notified in advance in the TTI boundary event.
  unsigned nof_slot_tti_in_advance;
  /// Logger.
  srslog::basic_logger* logger;
};

/// Implements a software generic lower PHY downlink baseband processor.
class downlink_processor_baseband_impl : public downlink_processor_baseband
{
public:
  /// \brief Constructs a software generic lower PHY downlink processor that can process downlink resource grids.
  /// \param[in] pdxch_proc_baseband_ PDxCH processor baseband.
  /// \param[in] amplitude_control_   Amplitude controller.
  /// \param[in] config               Downlink processor configuration.
  downlink_processor_baseband_impl(pdxch_processor_baseband&                        pdxch_proc_baseband_,
                                   amplitude_controller&                            amplitude_control_,
                                   const downlink_processor_baseband_configuration& config);

  // See interface for documentation.
  void connect(downlink_processor_notifier& notifier_) { notifier = &notifier_; }

  // See interface for documentation.
  void process(baseband_gateway_buffer_writer& buffer, baseband_gateway_timestamp timestamp) override;

private:
  /// Processes a new symbol.
  void process_new_symbol(baseband_gateway_timestamp timestamp);

  /// Logger for printing amplitude control.
  srslog::basic_logger& amplitude_control_logger;
  /// PDxCH baseband processor.
  pdxch_processor_baseband& pdxch_proc_baseband;
  /// Amplitude control.
  amplitude_controller& amplitude_control;
  /// Number of slots notified in advanced in the TTI boundary.
  unsigned nof_slot_tti_in_advance;
  /// Sector identifier.
  unsigned sector_id;
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Number of receive ports.
  unsigned nof_rx_ports;
  /// Number of samples per subframe;
  unsigned nof_samples_per_subframe;
  /// Number of slots per subframe;
  unsigned nof_slots_per_subframe;
  /// Number of symbols per slot.
  unsigned nof_symbols_per_slot;
  /// \brief Reading index for the \ref temp_buffer holding OFDM symbols.
  ///
  /// Sample index within the \c temp_buffer data, it points the reading position within the generated signal. It is
  /// used to copy the samples aligned with the requested timestamp into the destination buffer.
  unsigned temp_buffer_read_index;
  /// List of the symbol sizes in number samples for each symbol within the subframe.
  std::vector<unsigned> symbol_sizes;
  /// Temporal storage of baseband samples.
  baseband_gateway_buffer_dynamic temp_buffer;
  /// Reference to the downlink notifier.
  downlink_processor_notifier* notifier = nullptr;
  /// Average symbol power statistics.
  sample_statistics<float> avg_symbol_power;
  /// Peak symbol power statistics.
  sample_statistics<float> peak_symbol_power;
  /// Symbol PAPR statistics.
  sample_statistics<float> symbol_papr;
};

} // namespace srsran
