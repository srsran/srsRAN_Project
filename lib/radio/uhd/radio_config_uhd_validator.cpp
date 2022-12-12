/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "radio_config_uhd_validator.h"
#include "fmt/format.h"
#include <regex>

using namespace srsgnb;

bool radio_config_uhd_config_validator::is_configuration_valid(const radio_configuration::radio& config) const
{
  return true;
}
