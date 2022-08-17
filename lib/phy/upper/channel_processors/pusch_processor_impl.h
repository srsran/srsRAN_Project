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
#include "srsgnb/phy/upper/signal_processors/dmrs_pusch_estimator.h"
#include <memory>

namespace srsgnb {

/// Describes the necessary parameters for creating a PUSCH processor.
struct pusch_processor_configuration {
  /// Channel estimator instance. Ownership is transferred to the processor.
  std::unique_ptr<dmrs_pusch_estimator> estimator;
  /// Demodulator instance. Ownership is transferred to the processor.
  std::unique_ptr<pusch_demodulator> demodulator;
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
  /// Channel estimator.
  std::unique_ptr<dmrs_pusch_estimator> estimator;
  /// PUSCH demodulator.
  std::unique_ptr<pusch_demodulator> demodulator;
  /// PUSCH decoder.
  std::unique_ptr<pusch_decoder> decoder;
  /// Temporal channel estimate.
  channel_estimate ch_estimate;
  /// Codeword LLR buffer.
  std::array<log_likelihood_ratio, pusch_demodulator::MAX_NOF_DATA_LLR> temp_codeword_llr;
  /// HARQ-ACK LLR buffer.
  std::array<log_likelihood_ratio, pusch_demodulator::MAX_NOF_HARQ_ACK_LLR> temp_harq_ack_llr;
  /// CSI Part1 LLR buffer.
  std::array<log_likelihood_ratio, pusch_demodulator::MAX_NOF_CSI_PART1_LLR> temp_csi_part1_llr;
  /// CSI Part2 LLR buffer type.
  std::array<log_likelihood_ratio, pusch_demodulator::MAX_NOF_CSI_PART2_LLR> temp_csi_part2_llr;
};

} // namespace srsgnb
