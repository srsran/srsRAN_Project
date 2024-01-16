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

namespace detail {

/// Baseband buffer that stores a single OFDM symbol.
class baseband_symbol_buffer
{
public:
  /// Constructs a baseband symbol buffer from a number of ports and an initial symbol size.
  baseband_symbol_buffer(unsigned nof_ports_, unsigned symbol_size) :
    buffer(nof_ports_, symbol_size), nof_ports(nof_ports_)
  {
    empty = true;
  }

  /// Clears the symbol buffer, allowing it to be written.
  void clear()
  {
    // Mark buffer as empty.
    empty = true;
  }

  /// \brief Reads samples from the buffer, starting at an initial timestamp.
  ///
  /// Copies the samples starting at the sample indicated by \c timestamp until \out is filled or until all samples
  /// have been read.
  ///
  /// \param[out] out Destination buffer.
  /// \param[in] timestamp Timestamp of the first sample to read.
  /// \return The number of samples read from the buffer.
  /// \remark \ref write_symbol must be called before attempting to read from the buffer.
  /// \remark The number of ports of \c out must match the number of ports of the symbol buffer.
  unsigned read(baseband_gateway_buffer_writer& out, baseband_gateway_timestamp timestamp)
  {
    srsran_assert(!empty, "Attempting to read from empty buffer.");
    srsran_assert(out.get_nof_channels() == nof_ports, "Number of ports does not match.");

    // Clear the buffer if attempting to read samples in the past.
    if (timestamp < symbol_start_timestamp) {
      clear();
      return 0;
    }

    unsigned read_index = timestamp - symbol_start_timestamp;

    // Clear the buffer if attempting to read samples in the future.
    if (read_index >= buffer.get_nof_samples()) {
      clear();
      return 0;
    }

    // Select the minimum among the samples to process and the number of stored samples that have not been read.
    unsigned nof_available_samples = buffer.get_nof_samples() - read_index;
    unsigned count                 = std::min(out.get_nof_samples(), nof_available_samples);

    // For each port, concatenate samples.
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      // Select view of the temporary buffer.
      span<const cf_t> temp_buffer_src = buffer[i_port].subspan(read_index, count);

      // Select view of the output samples.
      span<cf_t> temp_buffer_dst = out.get_channel_buffer(i_port).first(count);

      srsvec::copy(temp_buffer_dst, temp_buffer_src);
    }

    // Clear the buffer if all available samples have been read.
    if (count == nof_available_samples) {
      clear();
    }

    // Return the number of read samples.
    return count;
  }

  /// \brief Provides write access into the symbol buffer.
  ///
  /// Allows writing into the symbol buffer by means of a writing interface. It also sets the timestamp of the symbol
  /// stored in the buffer.
  ///
  /// \param[in] symbol_timestamp Timestamp of the symbol to write.
  /// \param[in] symbol_size Number of samples to write.
  /// \return A buffer writing interface that can be used to write samples into the buffer.
  /// \remark The buffer must be cleared by calling \ref clear or by reading all of its contents between writes.
  baseband_gateway_buffer_writer& write_symbol(baseband_gateway_timestamp symbol_timestamp, unsigned symbol_size)
  {
    srsran_assert(empty, "Attempting to write into non-empty buffer.");
    srsran_assert(symbol_size > 0, "Symbol size cannot be zero.");

    buffer.resize(symbol_size);
    symbol_start_timestamp = symbol_timestamp;
    empty                  = false;

    return buffer.get_writer();
  }

  /// \brief Gets the number of available samples for a given initial timestamp.
  ///
  /// Computes the number of samples stored in the buffer, starting at the sample indicated by \c timestamp. It
  /// returns 0 if the buffer has been cleared or if \c timestamp belongs to a different symbol.
  ///
  /// \param[in] timestamp Timestamp of the first sample that must be included in the count.
  /// \return The number of available samples starting at \c timestamp.
  unsigned get_nof_available_samples(baseband_gateway_timestamp timestamp) const
  {
    // No available samples if the buffer is empty, or for timestamps in the past or in the future.
    if (empty || (timestamp < symbol_start_timestamp) ||
        (timestamp >= symbol_start_timestamp + buffer.get_nof_samples())) {
      return 0;
    }

    unsigned read_index = timestamp - symbol_start_timestamp;
    return buffer.get_nof_samples() - read_index;
  }

private:
  /// Storage of baseband samples.
  baseband_gateway_buffer_dynamic buffer;

  /// Timestamp of the first baseband sample stored in the temporary buffer.
  baseband_gateway_timestamp symbol_start_timestamp;
  /// Number of ports of the buffer.
  unsigned nof_ports;
  /// Indicates if the buffer contents are outdated and can be overwritten.
  bool empty;
};

} // namespace detail

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
  baseband_gateway_transmitter_metadata process(baseband_gateway_buffer_writer& buffer,
                                                baseband_gateway_timestamp      timestamp) override;

private:
  /// \brief Processes a new symbol.
  ///
  /// \param[out] buffer Destination buffer.
  /// \param[in]  slot Slot number.
  /// \param[in]  i_symbol Symbol number within the current slot.
  /// \return \c true if the symbol has been processed, \c false otherwise.
  bool process_new_symbol(baseband_gateway_buffer_writer& buffer, slot_point slot, unsigned i_symbol);

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
  /// List of the symbol sizes in number samples for each symbol within the subframe.
  std::vector<unsigned> symbol_sizes;
  /// Reference to the downlink notifier.
  downlink_processor_notifier* notifier = nullptr;
  /// Average symbol power statistics.
  sample_statistics<float> avg_symbol_power;
  /// Peak symbol power statistics.
  sample_statistics<float> peak_symbol_power;
  /// Symbol PAPR statistics.
  sample_statistics<float> symbol_papr;
  /// Temporal storage of baseband samples.
  detail::baseband_symbol_buffer temp_buffer;
  /// Last notified slot boundary.
  optional<slot_point> last_notified_slot;
};

} // namespace srsran
