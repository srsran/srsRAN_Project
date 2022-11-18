/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/channel_processors/pusch_decoder.h"
#include "srsgnb/phy/upper/channel_processors/pusch_demodulator.h"
#include "srsgnb/phy/upper/channel_processors/pusch_processor.h"
#include "srsgnb/phy/upper/channel_processors/ulsch_demultiplex.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pusch_estimator.h"
#include <memory>

namespace srsgnb {

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
  /// Channel estimate dimensions.
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
  pusch_processor_result
  process(span<uint8_t> data, rx_softbuffer& softbuffer, const resource_grid_reader& grid, const pdu_t& pdu) override;

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

  /// \brief Maximum number of LLRs for CSI Part1 in a single transmission.
  ///
  /// As per TS38.212 Section 6.3.2.4.1.1 in \f$Q_{CSI-1}'\f$ formula. The value of \f$\sum
  /// ^{N_{symb,all}^{PUSCH}-1}_{l=l_0}M_{sc}^{UCI}(l)\f$ is bounded above by the maximum number of resource elements in
  /// a PUSCH transmission.
  static constexpr unsigned MAX_NOF_CSI_PART1_LLR =
      MAX_RB * pusch_constants::MAX_NRE_PER_RB * UCI_ON_PUSCH_MAX_ALPHA * pusch_constants::MAX_MODULATION_ORDER;

  /// \brief Maximum number of LLRs for CSI Part2 in a single transmission.
  ///
  /// As per TS38.212 Section 6.3.2.4.1.1 in \f$Q_{CSI-2}'\f$ formula. Ceiling the value of \f$\sum
  /// ^{N_{symb,all}^{PUSCH}-1}_{l=l_0}M_{sc}^{UCI}(l)\f$ to the maximum number of resource elements in a PUSCH
  /// transmission.
  static constexpr unsigned MAX_NOF_CSI_PART2_LLR =
      MAX_RB * pusch_constants::MAX_NRE_PER_RB * UCI_ON_PUSCH_MAX_ALPHA * pusch_constants::MAX_MODULATION_ORDER;

  /// Channel estimator.
  std::unique_ptr<dmrs_pusch_estimator> estimator;
  /// PUSCH demodulator.
  std::unique_ptr<pusch_demodulator> demodulator;
  /// UL-SCH demultiplexer.
  std::unique_ptr<ulsch_demultiplex> demultiplex;
  /// PUSCH decoder.
  std::unique_ptr<pusch_decoder> decoder;
  /// Temporal channel estimate.
  channel_estimate ch_estimate;
  /// Codeword LLR buffer.
  std::array<log_likelihood_ratio, pusch_demodulator::MAX_NOF_DATA_LLR> temp_codeword_llr;
  /// Shared channel LLR buffer.
  std::array<log_likelihood_ratio, pusch_demodulator::MAX_NOF_DATA_LLR> temp_sch_llr;
  /// HARQ-ACK LLR buffer.
  std::array<log_likelihood_ratio, MAX_NOF_HARQ_ACK_LLR> temp_harq_ack_llr;
  /// CSI Part1 LLR buffer.
  std::array<log_likelihood_ratio, MAX_NOF_CSI_PART1_LLR> temp_csi_part1_llr;
  /// CSI Part2 LLR buffer type.
  std::array<log_likelihood_ratio, MAX_NOF_CSI_PART2_LLR> temp_csi_part2_llr;
};

} // namespace srsgnb
