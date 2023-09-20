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

#include "srsran/phy/upper/channel_processors/pucch_demodulator.h"
#include "srsran/phy/upper/channel_processors/pucch_detector.h"
#include "srsran/phy/upper/channel_processors/pucch_processor.h"
#include "srsran/phy/upper/channel_processors/uci_decoder.h"
#include "srsran/phy/upper/signal_processors/dmrs_pucch_processor.h"
#include "srsran/ran/pucch/pucch_constants.h"

namespace srsran {

/// Implements a parameter validator for \ref pucch_processor_impl.
class pucch_pdu_validator_impl : public pucch_pdu_validator
{
public:
  /// \brief Constructs a PUCCH processor validator.
  ///
  /// The channel estimate dimensions are used to set the bandwidth, slot duration, number of receive ports and transmit
  /// layers.
  ///
  /// \param[in] ce_dims_ Provides the channel estimates dimensions.
  explicit pucch_pdu_validator_impl(const channel_estimate::channel_estimate_dimensions& ce_dims_) : ce_dims(ce_dims_)
  {
    // Do nothing.
  }

  // See interface for documentation.
  bool is_valid(const pucch_processor::format0_configuration& config) const override { return true; };
  bool is_valid(const pucch_processor::format1_configuration& config) const override;
  bool is_valid(const pucch_processor::format2_configuration& config) const override;
  bool is_valid(const pucch_processor::format3_configuration& config) const override { return true; }
  bool is_valid(const pucch_processor::format4_configuration& config) const override { return true; }

private:
  /// Maximum transmit and receive resource grid dimensions handled by the PUCCH processor.
  channel_estimate::channel_estimate_dimensions ce_dims;
};

/// Implementation of the PUCCH processor interface.
class pucch_processor_impl : public pucch_processor
{
public:
  /// Maximum supported PUCCH Format 2 UCI payload length in number of bits.
  static constexpr unsigned FORMAT2_MAX_UCI_NBITS = 1706;

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
    estimates(estimates_dimensions),
    max_sizes(estimates_dimensions)
  {
    srsran_assert(channel_estimator_format_1, "Invalid channel estimator for PUCCH Format 1.");
    srsran_assert(channel_estimator_format_2, "Invalid channel estimator for PUCCH Format 2.");
    srsran_assert(detector, "Invalid detector.");
    srsran_assert(demodulator, "Invalid PUCCH demodulator.");
    srsran_assert(decoder, "Invalid UCI decoder.");
  }

private:
  /// Validates PUCCH Format 1 configuration.
  void assert_format1_config(const pucch_processor::format1_configuration& config);
  /// Validates PUCCH Format 2 configuration.
  void assert_format2_config(const pucch_processor::format2_configuration& config);

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
  /// Maximum RB, symbol and channel sizes handled by the processor.
  channel_estimate::channel_estimate_dimensions max_sizes;
  /// Temporal LLR storage.
  std::array<log_likelihood_ratio, pucch_constants::MAX_NOF_LLR> temp_llr;
};

} // namespace srsran
