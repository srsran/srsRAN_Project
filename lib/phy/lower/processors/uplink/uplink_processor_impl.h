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
#include "srsran/adt/tensor.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
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
  /// States.
  enum class fsm_states {
    /// The processor is waiting to receive the next subframe boundary.
    alignment,
    /// The processor baseband buffering is synchronized and it is collecting samples.
    collecting
  };

  // See interface for documentation.
  void process(const baseband_gateway_buffer_reader& samples, baseband_gateway_timestamp timestamp) override;

  /// \brief Processes samples in alignment state.
  /// \param[in] samples   Input baseband samples.
  /// \param[in] timestamp Time instant in which the first sample within \c samples was received.
  void process_alignment(const baseband_gateway_buffer_reader& samples, baseband_gateway_timestamp timestamp);

  /// \brief Processes a symbol boundary.
  /// \param[in] samples   Input baseband samples.
  /// \param[in] timestamp Time instant in which the first sample within \c samples was received.
  void process_symbol_boundary(const baseband_gateway_buffer_reader& samples, baseband_gateway_timestamp timestamp);

  /// \brief Collects symbol samples.
  /// \param[in] samples   Input baseband samples.
  /// \param[in] timestamp Time instant in which the first sample within \c samples was received.
  void process_collecting(const baseband_gateway_buffer_reader& samples, baseband_gateway_timestamp timestamp);

  /// Finite state machine state.
  fsm_states state = fsm_states::alignment;
  /// Sector identifier.
  unsigned sector_id;
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Number of receive ports.
  unsigned nof_rx_ports;
  /// Number of slots per subframe.
  unsigned nof_slots_per_subframe;
  /// Number of symbols per slot.
  unsigned nof_symbols_per_slot;
  /// Number of samples per subframe.
  unsigned nof_samples_per_subframe;
  /// Number of symbols per subframe.
  unsigned nof_symbols_per_subframe;
  /// \brief Write index for the \ref temp_buffer holding OFDM symbols.
  ///
  /// Sample index within the \c temp_buffer data, it points the writing position within the buffered signal. It is
  /// used to copy the samples aligned with the requested timestamp into the destination buffer.
  unsigned temp_buffer_write_index;
  /// Current symbol index within the slot.
  unsigned current_symbol_index;
  /// Current symbol size.
  unsigned current_symbol_size;
  /// Current symbol timestamp.
  baseband_gateway_timestamp current_symbol_timestamp;
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
