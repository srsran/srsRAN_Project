/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "radio_uhd_device.h"
#include "radio_uhd_multi_usrp.h"
#include "srsgnb/radio/radio_configuration.h"

namespace srsgnb {

/// Radio configuration validator for uhd based radios.
class radio_config_uhd_config_validator : public radio_configuration::validator
{
public:
  // Set device properties.
  void set_properties(radio_uhd_device::properties properties_) { properties = std::move(properties_); }

  // See interface for documentation.
  bool is_configuration_valid(const radio_configuration::radio& config) const override;

private:
  radio_uhd_device::properties properties;
};

} // namespace srsgnb
