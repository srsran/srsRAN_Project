/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/expected.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_demodulator.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_detector.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor.h"
#include "srsran/phy/upper/channel_processors/uci/uci_decoder.h"
#include "srsran/phy/upper/signal_processors/pucch/dmrs_pucch_estimator.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"

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
  error_type<std::string> is_valid(const pucch_processor::format0_configuration& config) const override;
  error_type<std::string> is_valid(const pucch_processor::format1_configuration& config) const override;
  error_type<std::string> is_valid(const pucch_processor::format2_configuration& config) const override;
  error_type<std::string> is_valid(const pucch_processor::format3_configuration& config) const override;
  error_type<std::string> is_valid(const pucch_processor::format4_configuration& config) const override;

private:
  /// Maximum transmit and receive resource grid dimensions handled by the PUCCH processor.
  channel_estimate::channel_estimate_dimensions ce_dims;
};

/// Implementation of the PUCCH processor interface.
class pucch_processor_impl : public pucch_processor
{
public:
  // See pucch_processor interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format0_configuration& config) override;

  // See pucch_processor interface for documentation.
  pucch_format1_map<pucch_processor_result> process(const resource_grid_reader&        grid,
                                                    const format1_batch_configuration& config) override;

  // See pucch_processor interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format2_configuration& config) override;

  // See pucch_processor interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format3_configuration& config) override;

  // See pucch_processor interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format4_configuration& config) override;

  /// PUCCH processor constructor.
  pucch_processor_impl(std::unique_ptr<pucch_pdu_validator>                 pdu_validator_,
                       std::unique_ptr<dmrs_pucch_estimator>                channel_estimator_,
                       std::unique_ptr<pucch_detector>                      detector_,
                       std::unique_ptr<pucch_demodulator>                   demodulator_,
                       std::unique_ptr<uci_decoder>                         decoder_,
                       const channel_estimate::channel_estimate_dimensions& estimates_dimensions) :
    pdu_validator(std::move(pdu_validator_)),
    channel_estimator(std::move(channel_estimator_)),
    detector(std::move(detector_)),
    demodulator(std::move(demodulator_)),
    decoder(std::move(decoder_)),
    estimates(estimates_dimensions)
  {
    srsran_assert(channel_estimator, "Invalid channel estimator.");
    srsran_assert(detector, "Invalid detector.");
    srsran_assert(demodulator, "Invalid PUCCH demodulator.");
    srsran_assert(decoder, "Invalid UCI decoder.");
  }

private:
  /// PUCCH PDU validator for all formats.
  std::unique_ptr<pucch_pdu_validator> pdu_validator;
  /// Channel estimator.
  std::unique_ptr<dmrs_pucch_estimator> channel_estimator;
  /// PUCCH detector for 1 or 2 bits, using format 0 and 1.
  std::unique_ptr<pucch_detector> detector;
  /// PUCCH demodulator for more than 2 bits, using formats 2, 3 and 4.
  std::unique_ptr<pucch_demodulator> demodulator;
  /// UCI decoder for more than 2 bits, using formats 2, 3 and 4.
  std::unique_ptr<uci_decoder> decoder;
  /// Temporal channel estimates.
  channel_estimate estimates;
  /// Temporal LLR storage.
  std::array<log_likelihood_ratio, pucch_constants::MAX_NOF_LLR> temp_llr;
};

} // namespace srsran
