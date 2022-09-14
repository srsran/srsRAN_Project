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

#include "srsgnb/phy/upper/channel_processors/pucch_demodulator.h"
#include "srsgnb/phy/upper/channel_processors/pucch_detector.h"
#include "srsgnb/phy/upper/channel_processors/uci_decoder.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pucch_processor.h"
#include "uci_decoder.h"

namespace srsgnb {

class pucch_processor_impl : public pucch_processor
{
public:
  // See interface for documentation.
  pucch_processor_result process_format0(const resource_grid_reader& grid, const format0_configuration& config) override
  {
    pucch_detector::format0_configuration                   detector_config;
    std::pair<pucch_uci_message, channel_state_information> detection_result =
        detector->detect_format0(grid, detector_config);

    pucch_processor_result result;
    result.message = detection_result.first;
    result.csi     = detection_result.second;

    return result;
  }

  // See interface for documentation.
  pucch_processor_result process_format1(const resource_grid_reader& grid, const format1_configuration& config) override
  {
    pucch_processor_result result;

    dmrs_pucch_processor::config_t estimator_config;
    channel_estimator->estimate(estimates, grid, estimator_config);

    result.csi = estimates.get_channel_state_information();

    pucch_detector::format1_configuration detector_config;
    result.message = detector->detect_format1(grid, estimates, detector_config);

    return result;
  }

  // See interface for documentation.
  pucch_processor_result process_format2(const resource_grid_reader& grid, const format2_configuration& config) override
  {
    pucch_processor_result result;
    result.message.full_payload =
        span<uint8_t>(result.message.data)
            .first(config.common.nof_sr + config.common.nof_harq_ack + config.common.nof_csi_part1);
    result.message.harq_ack = result.message.full_payload.first(config.common.nof_harq_ack);
    result.message.csi_part1 =
        result.message.full_payload.subspan(config.common.nof_harq_ack, config.common.nof_csi_part1);
    result.message.sr =
        result.message.full_payload.subspan(config.common.nof_harq_ack + config.common.nof_sr, config.common.nof_sr);

    dmrs_pucch_processor::config_t estimator_config;
    channel_estimator->estimate(estimates, grid, estimator_config);

    result.csi = estimates.get_channel_state_information();

    span<log_likelihood_ratio>               llr = span<log_likelihood_ratio>(temp_llr).first(0);
    pucch_demodulator::format2_configuration demod_config;
    demodulator->demodulate_format2(llr, grid, estimates, demod_config);

    uci_decoder::configuration decoder_config;
    result.message.status = decoder->decode(result.message.full_payload, llr, decoder_config);

    return result;
  }

  // See interface for documentation.
  pucch_processor_result process_format3(const resource_grid_reader& grid, const format3_configuration& config) override
  {
    // TBD.
    return pucch_processor_result();
  }

  // See interface for documentation.
  pucch_processor_result process_format4(const resource_grid_reader& grid, const format4_configuration& config) override
  {
    // TBD.
    return pucch_processor_result();
  }

private:
  /// PUCCH detector for 1 or 2 bits, using format 0 and 1.
  std::unique_ptr<pucch_detector> detector;
  /// PUCCH demodulator for more than 2 bits, using formats 2, 3 and 4.
  std::unique_ptr<pucch_demodulator> demodulator;
  /// UCI decoder for more than 2 bits, using formats 2, 3 and 4.
  std::unique_ptr<uci_decoder> decoder;
  /// Channel estimator.
  std::unique_ptr<dmrs_pucch_processor> channel_estimator;
  /// Temporal channel estimates.
  channel_estimate estimates;
  /// Temporal LLR storage.
  std::array<log_likelihood_ratio, PUCCH_MAX_NOF_LLR> temp_llr;
};

} // namespace srsgnb
