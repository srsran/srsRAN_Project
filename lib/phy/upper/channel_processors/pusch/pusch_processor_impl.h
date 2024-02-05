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

#include "pusch_processor_notifier_adaptor.h"
#include "pusch_uci_decoder_notifier.h"
#include "pusch_uci_decoder_wrapper.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "srsran/phy/upper/channel_processors/pusch/ulsch_demultiplex.h"
#include "srsran/phy/upper/channel_processors/uci/uci_decoder.h"
#include "srsran/phy/upper/signal_processors/dmrs_pusch_estimator.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/ran/pusch/ulsch_info.h"
#include "srsran/support/memory_pool/concurrent_thread_local_object_pool.h"
#include <memory>

namespace srsran {

/// Implements a parameter validator for \ref pusch_processor_impl.
class pusch_processor_validator_impl : public pusch_pdu_validator
{
public:
  /// \brief Constructs PUSCH processor validator.
  ///
  /// It requires channel estimate dimensions to limit the bandwidth, slot duration, number of receive ports and
  /// transmit layers.
  ///
  /// \param[in] ce_dims_ Provides the channel estimates dimensions.
  explicit pusch_processor_validator_impl(const channel_estimate::channel_estimate_dimensions& ce_dims_) :
    ce_dims(ce_dims_)
  {
    srsran_assert(
        (ce_dims.nof_prb > 0) && (ce_dims.nof_prb <= MAX_RB), "Invalid number of PRB (i.e. {}).", ce_dims.nof_prb);
    srsran_assert((ce_dims.nof_symbols > 0) && (ce_dims.nof_symbols <= MAX_NSYMB_PER_SLOT),
                  "Invalid number of OFDM symbols.");
    srsran_assert(ce_dims.nof_rx_ports > 0, "The number of receive ports cannot be zero.");
    srsran_assert(ce_dims.nof_tx_layers == 1, "Only one transmit layer is currently supported.");
  }

  // See interface for documentation.
  bool is_valid(const pusch_processor::pdu_t& pdu) const override;

private:
  channel_estimate::channel_estimate_dimensions ce_dims;
};

/// Implements a generic software PUSCH processor.
class pusch_processor_impl : public pusch_processor
{
public:
  /// Groups the PUSCH processor dependencies that can be reused locally by the same processing thread.
  class concurrent_dependencies
  {
  public:
    /// Creates the dependencies instance.
    concurrent_dependencies(std::unique_ptr<dmrs_pusch_estimator>         estimator_,
                            std::unique_ptr<pusch_demodulator>            demodulator_,
                            std::unique_ptr<ulsch_demultiplex>            demultiplex_,
                            std::unique_ptr<uci_decoder>                  uci_dec_,
                            channel_estimate::channel_estimate_dimensions ce_dims) :
      estimator(std::move(estimator_)),
      demodulator(std::move(demodulator_)),
      demultiplex(std::move(demultiplex_)),
      uci_dec(std::move(uci_dec_)),
      ch_estimate(ce_dims),
      harq_ack_decoder(*uci_dec,
                       pusch_constants::get_max_codeword_size(ce_dims.nof_prb, ce_dims.nof_tx_layers).value()),
      csi_part1_decoder(*uci_dec,
                        pusch_constants::get_max_codeword_size(ce_dims.nof_prb, ce_dims.nof_tx_layers).value()),
      csi_part2_decoder(*uci_dec,
                        pusch_constants::get_max_codeword_size(ce_dims.nof_prb, ce_dims.nof_tx_layers).value())

    {
      srsran_assert(estimator, "Invalid channel estimator.");
      srsran_assert(demodulator, "Invalid demodulator.");
      srsran_assert(demultiplex, "Invalid demultiplex.");
      srsran_assert(uci_dec, "Invalid UCI decoder.");
    }

    dmrs_pusch_estimator&      get_estimator() { return *estimator; }
    pusch_demodulator&         get_demodulator() { return *demodulator; }
    ulsch_demultiplex&         get_demultiplex() { return *demultiplex; }
    pusch_uci_decoder_wrapper& get_harq_ack_decoder() { return harq_ack_decoder; }
    pusch_uci_decoder_wrapper& get_csi_part1_decoder() { return csi_part1_decoder; }
    pusch_uci_decoder_wrapper& get_csi_part2_decoder() { return csi_part2_decoder; }
    channel_estimate&          get_channel_estimate() { return ch_estimate; }

  private:
    /// Channel estimator instance.
    std::unique_ptr<dmrs_pusch_estimator> estimator;
    /// Demodulator instance.
    std::unique_ptr<pusch_demodulator> demodulator;
    /// Channel demultiplex.
    std::unique_ptr<ulsch_demultiplex> demultiplex;
    /// UCI Decoder instance.
    std::unique_ptr<uci_decoder> uci_dec;
    /// Temporal channel estimate.
    channel_estimate ch_estimate;
    /// HARQ-ACK decoder wrapper.
    pusch_uci_decoder_wrapper harq_ack_decoder;
    /// CSI Part 1 decoder wrapper.
    pusch_uci_decoder_wrapper csi_part1_decoder;
    /// CSI Part 2 decoder wrapper.
    pusch_uci_decoder_wrapper csi_part2_decoder;
  };

  /// Dependencies pool class.
  using concurrent_dependencies_pool_type = concurrent_thread_local_object_pool<concurrent_dependencies>;

  /// Collects the necessary parameters for creating a PUSCH processor.
  struct configuration {
    /// Thread local dependencies pool.
    std::shared_ptr<concurrent_dependencies_pool_type> thread_local_dependencies_pool;
    /// Decoder instance. Ownership is transferred to the processor.
    std::unique_ptr<pusch_decoder> decoder;
    /// Selects the number of LDPC decoder iterations.
    unsigned dec_nof_iterations;
    /// Enables LDPC decoder early stop if the CRC matches before completing \c ldpc_nof_iterations iterations.
    bool dec_enable_early_stop;
    /// PUSCH SINR calculation method for CSI reporting.
    channel_state_information::sinr_type csi_sinr_calc_method;
  };

  /// \brief Constructs a generic software PUSCH processor.
  /// \param[in] config PUSCH processor dependencies and configuration parameters.
  pusch_processor_impl(configuration& config);

  // See interface for documentation.
  void process(span<uint8_t>                    data,
               unique_rx_buffer                 rm_buffer,
               pusch_processor_result_notifier& notifier,
               const resource_grid_reader&      grid,
               const pdu_t&                     pdu) override;

private:
  /// Asserts the PDU. It triggers an assertion upon an invalid value or combination of values.
  void assert_pdu(const pusch_processor::pdu_t& pdu, const channel_estimate& ch_estimate) const;

  /// Thread local dependencies pool.
  std::shared_ptr<concurrent_dependencies_pool_type> thread_local_dependencies_pool;
  /// UL-SCH transport block decoder.
  std::unique_ptr<pusch_decoder> decoder;
  /// Selects the number of LDPC decoder iterations.
  unsigned dec_nof_iterations;
  /// Enables LDPC decoder early stop if the CRC matches before completing \c ldpc_nof_iterations iterations.
  bool dec_enable_early_stop;
  /// Selects the PUSCH SINR calculation method.
  channel_state_information::sinr_type csi_sinr_calc_method;
  /// Notifier adaptor.
  pusch_processor_notifier_adaptor notifier_adaptor;
};

} // namespace srsran
