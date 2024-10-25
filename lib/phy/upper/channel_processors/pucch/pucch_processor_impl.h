/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_demodulator.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_detector.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor.h"
#include "srsran/phy/upper/channel_processors/uci/uci_decoder.h"
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
  error_type<std::string> is_valid(const pucch_processor::format0_configuration& config) const override;
  error_type<std::string> is_valid(const pucch_processor::format1_configuration& config) const override;
  error_type<std::string> is_valid(const pucch_processor::format2_configuration& config) const override;
  error_type<std::string> is_valid(const pucch_processor::format3_configuration& config) const override
  {
    return default_success_t();
  }
  error_type<std::string> is_valid(const pucch_processor::format4_configuration& config) const override
  {
    return default_success_t();
  }

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
  pucch_processor_result process(const resource_grid_reader& grid, const format0_configuration& config) override;

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format1_configuration& config) override;

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format2_configuration& config) override;

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format3_configuration& config) override
  {
    // TBD.
    return {};
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format4_configuration& config) override
  {
    // TBD.
    return {};
  }

  /// PUCCH processor constructor.
  pucch_processor_impl(std::unique_ptr<pucch_pdu_validator>                 pdu_validator_,
                       std::unique_ptr<dmrs_pucch_processor>                channel_estimator_format_1_,
                       std::unique_ptr<dmrs_pucch_processor>                channel_estimator_format_2_,
                       std::unique_ptr<pucch_detector>                      detector_,
                       std::unique_ptr<pucch_demodulator>                   demodulator_,
                       std::unique_ptr<uci_decoder>                         decoder_,
                       const channel_estimate::channel_estimate_dimensions& estimates_dimensions) :
    pdu_validator(std::move(pdu_validator_)),
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
  /// PUCCH PDU validator for all formats.
  std::unique_ptr<pucch_pdu_validator> pdu_validator;
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