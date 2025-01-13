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

#include "srs_validator_generic_impl.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration.h"

using namespace srsran;

error_type<std::string> srs_validator_generic_impl::is_valid(const srs_estimator_configuration& config) const
{
  // Check the SRS resource is valid.
  if (!config.resource.is_valid()) {
    return make_unexpected("Invalid SRS resource configuration.");
  }

  // Frequency hopping is not supported.
  if (config.resource.has_frequency_hopping()) {
    return make_unexpected("The SRS estimator does not support frequency hopping.");
  }

  // Sequence and group hopping is not supported.
  if (config.resource.hopping != srs_resource_configuration::group_or_sequence_hopping_enum::neither) {
    return make_unexpected("The SRS estimator does not support group or sequence hopping.");
  }

  // Invalid receive port list.
  if (config.ports.empty()) {
    return make_unexpected("Empty list of Rx ports for the SRS estimator.");
  }

  return default_success_t();
}
