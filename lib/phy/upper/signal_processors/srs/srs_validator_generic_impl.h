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

#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration_validator.h"

namespace srsran {

/// \brief Implements a Sounding Reference Signal channel estimator configuration validator.
///
/// The validator is aligned with \ref srs_estimator_generic_impl implementation of \ref srs_estimator.
class srs_validator_generic_impl : public srs_estimator_configuration_validator
{
public:
  /// Constructs an SRS estimator configuration validator with the maximum number of PRB.
  srs_validator_generic_impl(unsigned max_nof_prb_) : max_nof_prb(max_nof_prb_) {}

  // See interface for documentation.
  error_type<std::string> is_valid(const srs_estimator_configuration& config) const override;

private:
  unsigned max_nof_prb;
};

} // namespace srsran
