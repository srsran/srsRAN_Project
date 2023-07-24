/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/ofh/ofh_constants.h"
#include "srsran/ru/ru_ofh_configuration.h"

using namespace srsran;

static bool check_compression_params(const ofh::ru_compression_params& params)
{
  if (!(params.type == ofh::compression_type::none) && !(params.type == ofh::compression_type::BFP)) {
    fmt::print("Compression method not supported. Valid values [none,bfp].\n");
    return false;
  }

  if (params.type == ofh::compression_type::BFP && !(params.data_width == 8) && !(params.data_width == 9) &&
      !(params.data_width == 12) && !(params.data_width == 14) && !(params.data_width == 16)) {
    fmt::print("BFP compression bit width not supported. Valid values [8,9,12,14,16].\n");

    return false;
  }

  return true;
}

static bool check_dl_eaxc_if_broadcast_is_enabled(const ru_ofh_sector_configuration& config)
{
  // When broadcast flag is enabled, two downlink eAxCs are supported.
  if (config.is_downlink_broadcast_enabled && config.dl_eaxc.size() != 2) {
    fmt::print("Invalid downlink eAxC identifier configuration, broadcast flag is {} and there are {} downlink eAxC\n",
               (config.is_downlink_broadcast_enabled) ? "enabled" : "disabled",
               config.dl_eaxc.size());
    return false;
  }

  return true;
}

static bool check_eaxc_id(unsigned eaxc)
{
  bool result = eaxc < ofh::MAX_SUPPORTED_EAXC_ID_VALUE;
  if (!result) {
    fmt::print("eAxC id={} not supported. Valid values [0-{}]\n", eaxc, ofh::MAX_SUPPORTED_EAXC_ID_VALUE - 1U);
  }

  return result;
}

static bool check_eaxcs_id(const ru_ofh_sector_configuration& config)
{
  // Check PRACH eAxC.
  for (auto eaxc : config.prach_eaxc) {
    if (!check_eaxc_id(eaxc)) {
      return false;
    }
  }

  // Check uplink eAxCs.
  for (auto eaxc : config.ul_eaxc) {
    if (!check_eaxc_id(eaxc)) {
      return false;
    }
  }

  // Check downlink eAxCs.
  for (auto eaxc : config.dl_eaxc) {
    if (!check_eaxc_id(eaxc)) {
      return false;
    }
  }

  return true;
}

bool srsran::is_valid_ru_ofh_config(const ru_ofh_configuration& config)
{
  for (const auto& sector : config.sector_configs) {
    if (!check_compression_params(sector.ul_compression_params)) {
      return false;
    }

    if (!check_compression_params(sector.dl_compression_params)) {
      return false;
    }

    if (!check_compression_params(sector.prach_compression_params)) {
      return false;
    }

    if (!check_dl_eaxc_if_broadcast_is_enabled(sector)) {
      return false;
    }

    if (!check_eaxcs_id(sector)) {
      return false;
    }
  }

  return true;
}
