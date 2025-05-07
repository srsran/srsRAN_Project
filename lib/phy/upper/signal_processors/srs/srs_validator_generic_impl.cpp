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
#include "srsran/ran/resource_block.h"
#include "srsran/ran/srs/srs_information.h"

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

  for (unsigned i_port = 0, i_port_end = config.ports.size(); i_port != i_port_end; ++i_port) {
    srs_information info = get_srs_information(config.resource, i_port);

    if (info.mapping_initial_subcarrier + (info.sequence_length - 1) * info.comb_size >=
        max_nof_prb * NOF_SUBCARRIERS_PER_RB) {
      return make_unexpected("SRS resource exceeds maximum bandwidth.");
    }
  }

  return default_success_t();
}
