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

#include "gnb_appconfig.h"
#include "srsgnb/phy/lower/lower_phy_configuration.h"
#include "srsgnb/radio/radio_configuration.h"
#include "srsgnb/scheduler/config/cell_config_builder_params.h"

namespace srsgnb {

/// Builds and returns a default monolithic gnB application configuration with the parameters specified for the given
/// profile.
gnb_appconfig build_profile_gnb_appconfig(const std::string& profile_name);

struct configuration_profile {
  std::string name;
  std::string description;
};

static const std::vector<configuration_profile> supported_profiles = {
    {"zmq_20MHz_n7", "Single 20MHz FDD in band n7 using ZMQ."},
    {"uhd_20MHz_n7", "Single 20MHz FDD in band n7 using UHD."},
    {"zmq_20MHz_n41", "Single 20MHz TDD in band n41 using ZMQ."}};

} // namespace srsgnb