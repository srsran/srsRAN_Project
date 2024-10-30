/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
