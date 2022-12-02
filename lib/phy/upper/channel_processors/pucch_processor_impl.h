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
#include "srsgnb/phy/upper/channel_processors/pucch_processor.h"
#include "srsgnb/phy/upper/channel_processors/uci_decoder.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pucch_processor.h"

namespace srsgnb {

/// Implements a parameter validator for \ref pucch_processor_impl.
class pucch_processor_validator_impl : public pucch_pdu_validator
{
public:
  // See interface for documentation.
  bool is_valid(const pucch_processor::format0_configuration& config) override { return true; }
  bool is_valid(const pucch_processor::format1_configuration& config) override { return true; }
  bool is_valid(const pucch_processor::format2_configuration& config) override { return true; }
  bool is_valid(const pucch_processor::format3_configuration& config) override { return true; }
  bool is_valid(const pucch_processor::format4_configuration& config) override { return true; }
};

/// Implementation of the PUCCH processor interface.
class pucch_processor_impl : public pucch_processor
{
public:
  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format0_configuration& config) override
  {
    pucch_detector::format0_configuration                   detector_config;
    std::pair<pucch_uci_message, channel_state_information> detection_result = detector->detect(grid, detector_config);

    pucch_processor_result result;
    result.message = detection_result.first;
    result.csi     = detection_result.second;

    return result;
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format1_configuration& config) override;

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format2_configuration& config) override;

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format3_configuration& config) override
  {
    // TBD.
    return pucch_processor_result();
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format4_configuration& config) override
  {
    // TBD.
    return pucch_processor_result();
  }

  /// PUCCH processor constructor.
  pucch_processor_impl(std::unique_ptr<dmrs_pucch_processor>                channel_estimator_format_1_,
                       std::unique_ptr<dmrs_pucch_processor>                channel_estimator_format_2_,
                       std::unique_ptr<pucch_detector>                      detector_,
                       std::unique_ptr<pucch_demodulator>                   demodulator_,
                       std::unique_ptr<uci_decoder>                         decoder_,
                       const channel_estimate::channel_estimate_dimensions& estimates_dimensions) :
    channel_estimator_format_1(std::move(channel_estimator_format_1_)),
    channel_estimator_format_2(std::move(channel_estimator_format_2_)),
    detector(std::move(detector_)),
    demodulator(std::move(demodulator_)),
    decoder(std::move(decoder_)),
    estimates(estimates_dimensions)
  {
    srsgnb_assert(channel_estimator_format_1, "Invalid channel estimator for PUCCH Format 1.");
    srsgnb_assert(channel_estimator_format_2, "Invalid channel estimator for PUCCH Format 2.");
    srsgnb_assert(detector, "Invalid detector.");
    srsgnb_assert(demodulator, "Invalid PUCCH demodulator.");
    srsgnb_assert(decoder, "Invalid UCI decoder.");
  }

private:
  /// Maximum supported UCI payload length in number of bits.
  static constexpr unsigned PUCCH_FORMAT2_MAX_UCI_LEN = 11;

  /// Validates PUCCH Format 2 configuration.
  static void assert_format2_config(const pucch_processor::format2_configuration& config);

  /// Channel estimator for PUCCH Format 1.
  std::unique_ptr<dmrs_pucch_processor> channel_estimator_format_1;
  /// Channel estimator for PUCCH Format 2.
  std::unique_ptr<dmrs_pucch_processor> channel_estimator_format_2;
  /// PUCCH detector for 1 or 2 bits, using format 0 and 1.
  std::unique_ptr<pucch_detector> detector;
  /// PUCCH demodulator for more than 2 bits, using formats 2, 3 and 4.
  std::unique_ptr<pucch_demodulator> demodulator;
  /// UCI decoder for more than 2 bits, using formats 2, 3 and 4.
  std::unique_ptr<uci_decoder> decoder;
  /// Temporal channel estimates.
  channel_estimate estimates;
  /// Temporal LLR storage.
  std::array<log_likelihood_ratio, PUCCH_MAX_NOF_LLR> temp_llr;
};

} // namespace srsgnb
