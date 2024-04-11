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

namespace srsran {

struct srs_estimator_configuration;

/// \brief Describes the Sounding Reference Signal channel estimator validator interface.
class srs_estimator_configuration_validator
{
public:
  /// Default destructor.
  virtual ~srs_estimator_configuration_validator() = default;

  /// \brief Validates SRS channel estimator configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const srs_estimator_configuration& config) const = 0;
};

} // namespace srsran
