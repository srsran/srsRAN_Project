/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "radio_uhd_device.h"
#include "radio_uhd_multi_usrp.h"
#include "srsran/radio/radio_configuration.h"

namespace srsran {

/// \brief Radio configuration validator for uhd based radios.
///
/// This validator validates that parameters values are generally valid, for example frequencies are valid numbers and
/// positive or stream arguments follow a certain pattern. However, it does not validate whether the actual values are
/// supported by the UHD device.
///
/// It is up to the user to select parameters within ranges. These can be consulted using the UHD application \e
/// uhd_usrp_probe.
class radio_config_uhd_config_validator : public radio_configuration::validator
{
public:
  // See interface for documentation.
  bool is_configuration_valid(const radio_configuration::radio& config) const override;
};

} // namespace srsran
