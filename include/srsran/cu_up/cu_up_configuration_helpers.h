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

#pragma once

#include "srsran/cu_up/cu_up_types.h"
#include "srsran/ran/qos/five_qi.h"
#include <map>

namespace srsran::config_helpers {

inline srs_cu_up::cu_up_qos_config
make_default_cu_up_custom_cfg_item(bool warn_on_drop, timer_duration metrics_period, bool test_mode)
{
  srs_cu_up::cu_up_qos_config cfg{};
  cfg.pdcp_custom_cfg = pdcp_custom_config{}; // defaults are configured as member-initialization within the struct.
  cfg.pdcp_custom_cfg.metrics_period  = metrics_period;
  cfg.pdcp_custom_cfg.tx.warn_on_drop = warn_on_drop;
  cfg.pdcp_custom_cfg.tx.test_mode    = test_mode;

  cfg.f1u_cfg = srs_cu_up::f1u_config{}; // defaults are configured as member-initialization within the struct.

  return cfg;
}

/// Generates default QoS configuration used by gNB CU-UP.
inline std::map<five_qi_t, srs_cu_up::cu_up_qos_config>
make_default_cu_up_qos_config_list(bool warn_on_drop, timer_duration metrics_period, bool test_mode)
{
  std::map<five_qi_t, srs_cu_up::cu_up_qos_config> qos_list = {};
  //
  // Guaranteed Bitrate 5QIs
  //
  {
    // 5QI=1
    qos_list[uint_to_five_qi(1)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=2
    qos_list[uint_to_five_qi(2)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=3
    qos_list[uint_to_five_qi(3)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=4
    qos_list[uint_to_five_qi(4)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=65
    qos_list[uint_to_five_qi(65)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=66
    qos_list[uint_to_five_qi(66)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=67
    qos_list[uint_to_five_qi(67)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  //
  // Non-Guaranteed Bitrate 5QIs
  //
  {
    // 5QI=5
    qos_list[uint_to_five_qi(5)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=6
    qos_list[uint_to_five_qi(6)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=7
    qos_list[uint_to_five_qi(7)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=8
    qos_list[uint_to_five_qi(8)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=9
    qos_list[uint_to_five_qi(9)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=10
    qos_list[uint_to_five_qi(10)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=69
    qos_list[uint_to_five_qi(69)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=70
    qos_list[uint_to_five_qi(70)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  //
  // Delay-critical Guaranteed Bitrate 5QIs
  //
  {
    // 5QI=82
    qos_list[uint_to_five_qi(82)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=83
    qos_list[uint_to_five_qi(83)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=84
    qos_list[uint_to_five_qi(84)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=85
    qos_list[uint_to_five_qi(85)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=86
    qos_list[uint_to_five_qi(86)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=87
    qos_list[uint_to_five_qi(87)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=88
    qos_list[uint_to_five_qi(88)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=89
    qos_list[uint_to_five_qi(89)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=90
    qos_list[uint_to_five_qi(90)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }

  return qos_list;
}

} // namespace srsran::config_helpers
