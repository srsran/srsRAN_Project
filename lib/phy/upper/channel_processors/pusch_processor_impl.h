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

#include "srsran/phy/upper/channel_processors/pusch_decoder.h"
#include "srsran/phy/upper/channel_processors/pusch_demodulator.h"
#include "srsran/phy/upper/channel_processors/pusch_processor.h"
#include "srsran/phy/upper/channel_processors/uci_decoder.h"
#include "srsran/phy/upper/channel_processors/ulsch_demultiplex.h"
#include "srsran/phy/upper/signal_processors/dmrs_pusch_estimator.h"
#include <memory>

namespace srsran {

/// Describes the necessary parameters for creating a PUSCH processor.
struct pusch_processor_configuration {
  /// Channel estimator instance. Ownership is transferred to the processor.
  std::unique_ptr<dmrs_pusch_estimator> estimator;
  /// Demodulator instance. Ownership is transferred to the processor.
  std::unique_ptr<pusch_demodulator> demodulator;
  /// Channel demultiplex. Ownership is transferred to the processor.
  std::unique_ptr<ulsch_demultiplex> demultiplex;
  /// Decoder instance. Ownership is transferred to the processor.
  std::unique_ptr<pusch_decoder> decoder;
  /// UCI Decoder instance. Ownership is transferred to the processor.
  std::unique_ptr<uci_decoder> uci_dec;
  /// Channel estimate dimensions.
  channel_estimate::channel_estimate_dimensions ce_dims;
  /// Selects the number of LDPC decoder iterations.
  unsigned dec_nof_iterations;
  /// Enables LDPC decoder early stop if the CRC matches before completing \c ldpc_nof_iterations iterations.
  bool dec_enable_early_stop;
};

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
  /// \brief Constructs a generic software PUSCH processor.
  /// \param[in] config PUSCH processor dependencies and configuration parameters.
  pusch_processor_impl(pusch_processor_configuration& config);

  // See interface for documentation.
  void process(span<uint8_t>                    data,
               rx_softbuffer&                   softbuffer,
               pusch_processor_result_notifier& notifier,
               const resource_grid_reader&      grid,
               const pdu_t&                     pdu) override;

private:
  /// Maximum UCI scaling \f$\alpha\f$ as per TS38.331 UCI-OnPUSCH.
  static constexpr unsigned UCI_ON_PUSCH_MAX_ALPHA = 1;

  /// \brief Maximum number of LLRs for HARQ-ACK in a single transmission.
  ///
  /// As per TS38.212 Section 6.3.2.4.1.1, \f$Q_{ACK}'\f$ formula. The value of \f$\sum
  /// ^{N_{symb,all}^{PUSCH}-1}_{l=l_0}M_{sc}^{UCI}(l)\f$ is bounded above by the maximum number of resource elements in
  /// a PUSCH transmission.
  static constexpr unsigned MAX_NOF_HARQ_ACK_LLR =
      MAX_RB * pusch_constants::MAX_NRE_PER_RB * UCI_ON_PUSCH_MAX_ALPHA * pusch_constants::MAX_MODULATION_ORDER;

  /// \brief Maximum number of LLRs for CSI Part 1 in a single transmission.
  ///
  /// As per TS38.212 Section 6.3.2.4.1.1 in \f$Q_{CSI-1}'\f$ formula. The value of \f$\sum
  /// ^{N_{symb,all}^{PUSCH}-1}_{l=l_0}M_{sc}^{UCI}(l)\f$ is bounded above by the maximum number of resource elements in
  /// a PUSCH transmission.
  static constexpr unsigned MAX_NOF_CSI_PART1_LLR =
      MAX_RB * pusch_constants::MAX_NRE_PER_RB * UCI_ON_PUSCH_MAX_ALPHA * pusch_constants::MAX_MODULATION_ORDER;

  /// \brief Maximum number of LLRs for CSI Part 2 in a single transmission.
  ///
  /// As per TS38.212 Section 6.3.2.4.1.1 in \f$Q_{CSI-2}'\f$ formula. Ceiling the value of \f$\sum
  /// ^{N_{symb,all}^{PUSCH}-1}_{l=l_0}M_{sc}^{UCI}(l)\f$ to the maximum number of resource elements in a PUSCH
  /// transmission.
  static constexpr unsigned MAX_NOF_CSI_PART2_LLR =
      MAX_RB * pusch_constants::MAX_NRE_PER_RB * UCI_ON_PUSCH_MAX_ALPHA * pusch_constants::MAX_MODULATION_ORDER;

  /// Asserts the PDU. It triggers an assertion upon an invalid value or combination of values.
  void assert_pdu(const pusch_processor::pdu_t& pdu) const;

  /// \brief Decodes a UCI field by reversing the channel coding steps described by TS38.212
  /// Sections 6.3.2.2, 6.3.2.3, 6.3.2.4, and 6.3.2.5.
  ///
  /// \param[in] llr            Input soft bits to decode.
  /// \param[in] nof_bits       Number of information bits to decode.
  /// \param[in] uci_dec_config UCI decoder configuration.
  /// \return The UCI field decoding result if the field is present. Otherwise, a UCI field with empty payload and
  /// unknown status.
  pusch_uci_field decode_uci_field(span<const log_likelihood_ratio>  llr,
                                   unsigned                          nof_bits,
                                   const uci_decoder::configuration& uci_dec_config);

  /// Channel estimator.
  std::unique_ptr<dmrs_pusch_estimator> estimator;
  /// PUSCH demodulator.
  std::unique_ptr<pusch_demodulator> demodulator;
  /// UL-SCH demultiplexer.
  std::unique_ptr<ulsch_demultiplex> demultiplex;
  /// UL-SCH transport block decoder.
  std::unique_ptr<pusch_decoder> decoder;
  /// UCI decoder.
  std::unique_ptr<uci_decoder> uci_dec;
  /// Temporal channel estimate.
  channel_estimate ch_estimate;
  /// Selects the number of LDPC decoder iterations.
  unsigned dec_nof_iterations;
  /// Enables LDPC decoder early stop if the CRC matches before completing \c ldpc_nof_iterations iterations.
  bool dec_enable_early_stop;
  /// Codeword LLR buffer.
  std::array<log_likelihood_ratio, pusch_demodulator::MAX_NOF_DATA_LLR> temp_codeword_llr;
  /// Shared channel LLR buffer.
  std::array<log_likelihood_ratio, pusch_demodulator::MAX_NOF_DATA_LLR> temp_sch_llr;
  /// HARQ-ACK LLR buffer.
  std::array<log_likelihood_ratio, MAX_NOF_HARQ_ACK_LLR> temp_harq_ack_llr;
  /// CSI Part 1 LLR buffer.
  std::array<log_likelihood_ratio, MAX_NOF_CSI_PART1_LLR> temp_csi_part1_llr;
  /// CSI Part 2 LLR buffer type.
  std::array<log_likelihood_ratio, MAX_NOF_CSI_PART2_LLR> temp_csi_part2_llr;
};

} // namespace srsran
