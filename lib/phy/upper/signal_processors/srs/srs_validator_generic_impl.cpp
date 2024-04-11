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

bool srs_validator_generic_impl::is_valid(const srs_estimator_configuration& config) const
{
  // Check the SRS resource is valid.
  if (!config.resource.is_valid()) {
    return false;
  }

  // Frequency hopping is not supported.
  if (config.resource.has_frequency_hopping()) {
    return false;
  }

  // Sequence and group hopping is not supported.
  if (config.resource.hopping != srs_resource_configuration::group_or_sequence_hopping_enum::neither) {
    return false;
  }

  // Invalid receive port list.
  if (config.ports.empty()) {
    return false;
  }

  return true;
}
