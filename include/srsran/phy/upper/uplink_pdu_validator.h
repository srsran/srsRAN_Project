/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/phy/upper/channel_processors/prach_detector.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration.h"

namespace srsran {

/// Uplink processor validation interface.
class uplink_pdu_validator
{
public:
  /// Default destructor.
  virtual ~uplink_pdu_validator() = default;

  /// \brief Validates PRACH detector configuration parameters.
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const prach_detector::configuration& config) const = 0;

  /// \brief Validates PUCCH Format 0 configuration parameters.
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const pucch_processor::format0_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 1 configuration parameters.
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const pucch_processor::format1_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 2 configuration parameters.
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const pucch_processor::format2_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 3 configuration parameters.
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const pucch_processor::format3_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 4 configuration parameters.
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const pucch_processor::format4_configuration& config) const = 0;

  /// \brief Validates PUSCH configuration parameters.
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const pusch_processor::pdu_t& config) const = 0;

  /// \brief Validates Sounding Reference Signals channel estimator configuration.
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const srs_estimator_configuration& config) const = 0;
};

} // namespace srsran
