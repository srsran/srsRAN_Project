/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ru/ru_ofh_configuration.h"

using namespace srsran;

static bool check_compression_params(const ofh::ru_compression_params& params)
{
  if (!(params.type == ofh::compression_type::none) && !(params.type == ofh::compression_type::BFP)) {
    fmt::print("Compression method not supported. Valid values [none,bfp].\n");
    return false;
  }

  if (!(params.data_width == 8) && !(params.data_width == 9) && !(params.data_width == 12) &&
      !(params.data_width == 14) && !(params.data_width == 16)) {
    fmt::print("Compression bit width not supported. Valid values [8,9,14,16].\n");

    return false;
  }

  return true;
}

static bool check_dl_ports_if_broadcast_is_enabled(const ru_ofh_configuration& config)
{
  if (!config.is_downlink_broadcast_enabled)
    return true;

  for (const auto& sector : config.sector_configs) {
    if (sector.ru_dl_ports.size() < 2) {
      fmt::print("If the broadcast flag is enabled, there must be more than one downlink port id configured.\n");
      return false;
    }
  }

  return true;
}

static bool check_port_id(unsigned port_id)
{
  static constexpr unsigned MAX_RU_PORT_ID = (1U << 5U);

  return port_id < MAX_RU_PORT_ID;
}

static bool check_ports_id(const ru_ofh_configuration& config)
{
  for (const auto& sector : config.sector_configs) {
    // Check PRACH port.
    if (!check_port_id(sector.ru_prach_port)) {
      return false;
    }

    // Check uplink port.
    if (!check_port_id(sector.ru_ul_port)) {
      return false;
    }

    // Check downlink ports
    for (auto port : sector.ru_dl_ports) {
      if (!check_port_id(port)) {
        return false;
      }
    }
  }

  return true;
}

bool srsran::is_valid_ru_ofh_config(const ru_ofh_configuration& config)
{
  if (!check_compression_params(config.ul_compression_params)) {
    return false;
  }

  if (!check_compression_params(config.dl_compression_params)) {
    return false;
  }

  if (!check_dl_ports_if_broadcast_is_enabled(config)) {
    return false;
  }

  if (!check_ports_id(config)) {
    return false;
  }

  return true;
}
