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

#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration_validator.h"

namespace srsran {

/// \brief Implements a Sounding Reference Signal channel estimator configuration validator.
///
/// The validator is aligned with \ref srs_estimator_generic_impl implementation of \ref srs_estimator.
class srs_validator_generic_impl : public srs_estimator_configuration_validator
{
public:
  // See interface for documentation.
  error_type<std::string> is_valid(const srs_estimator_configuration& config) const override;
};

} // namespace srsran
