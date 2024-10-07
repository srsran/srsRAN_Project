/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_up/cu_up_types.h"
#include "srsran/ran/qos/five_qi.h"
#include <map>

namespace srsran {
namespace config_helpers {

/// Generates default QoS configuration used by gNB CU-UP.
inline std::map<five_qi_t, srs_cu_up::cu_up_qos_config>
make_default_cu_up_qos_config_list(uint32_t       max_nof_crypto_workers,
                                   bool           warn_on_drop,
                                   timer_duration metrics_period,
                                   bool           test_mode)
{
  std::map<five_qi_t, srs_cu_up::cu_up_qos_config> qos_list = {};
  {
    // 5QI=1
    srs_cu_up::cu_up_qos_config cfg{};
    cfg.pdcp_custom_cfg = pdcp_custom_config{}; // defaults are configured as member-initialization within the struct
    cfg.pdcp_custom_cfg.metrics_period            = metrics_period;
    cfg.pdcp_custom_cfg.tx.max_nof_crypto_workers = max_nof_crypto_workers;
    cfg.pdcp_custom_cfg.tx.warn_on_drop           = warn_on_drop;
    cfg.pdcp_custom_cfg.tx.test_mode              = test_mode;
    cfg.pdcp_custom_cfg.rx.max_nof_crypto_workers = max_nof_crypto_workers;

    qos_list[uint_to_five_qi(1)] = cfg;
  }
  {
    // 5QI=2
    srs_cu_up::cu_up_qos_config cfg{};
    cfg.pdcp_custom_cfg = pdcp_custom_config{}; // defaults are configured as member-initialization within the struct
    cfg.pdcp_custom_cfg.metrics_period            = metrics_period;
    cfg.pdcp_custom_cfg.tx.max_nof_crypto_workers = max_nof_crypto_workers;
    cfg.pdcp_custom_cfg.tx.warn_on_drop           = warn_on_drop;
    cfg.pdcp_custom_cfg.tx.test_mode              = test_mode;
    cfg.pdcp_custom_cfg.rx.max_nof_crypto_workers = max_nof_crypto_workers;

    qos_list[uint_to_five_qi(2)] = cfg;
  }
  {
    // 5QI=5
    srs_cu_up::cu_up_qos_config cfg{};
    cfg.pdcp_custom_cfg = pdcp_custom_config{}; // defaults are configured as member-initialization within the struct
    cfg.pdcp_custom_cfg.metrics_period            = metrics_period;
    cfg.pdcp_custom_cfg.tx.max_nof_crypto_workers = max_nof_crypto_workers;
    cfg.pdcp_custom_cfg.tx.warn_on_drop           = warn_on_drop;
    cfg.pdcp_custom_cfg.tx.test_mode              = test_mode;
    cfg.pdcp_custom_cfg.rx.max_nof_crypto_workers = max_nof_crypto_workers;

    qos_list[uint_to_five_qi(5)] = cfg;
  }
  {
    // 5QI=7
    srs_cu_up::cu_up_qos_config cfg{};
    cfg.pdcp_custom_cfg = pdcp_custom_config{}; // defaults are configured as member-initialization within the struct
    cfg.pdcp_custom_cfg.metrics_period            = metrics_period;
    cfg.pdcp_custom_cfg.tx.max_nof_crypto_workers = max_nof_crypto_workers;
    cfg.pdcp_custom_cfg.tx.warn_on_drop           = warn_on_drop;
    cfg.pdcp_custom_cfg.tx.test_mode              = test_mode;
    cfg.pdcp_custom_cfg.rx.max_nof_crypto_workers = max_nof_crypto_workers;

    cfg.f1u_cfg.warn_on_drop = warn_on_drop;

    qos_list[uint_to_five_qi(7)] = cfg;
  }
  {
    // 5QI=9
    srs_cu_up::cu_up_qos_config cfg{};
    cfg.pdcp_custom_cfg = pdcp_custom_config{}; // defaults are configured as member-initialization within the struct
    cfg.pdcp_custom_cfg.metrics_period            = metrics_period;
    cfg.pdcp_custom_cfg.tx.max_nof_crypto_workers = max_nof_crypto_workers;
    cfg.pdcp_custom_cfg.tx.warn_on_drop           = warn_on_drop;
    cfg.pdcp_custom_cfg.tx.test_mode              = test_mode;
    cfg.pdcp_custom_cfg.rx.max_nof_crypto_workers = max_nof_crypto_workers;

    cfg.f1u_cfg.warn_on_drop = warn_on_drop;

    qos_list[uint_to_five_qi(9)] = cfg;
  }
  return qos_list;
}

} // namespace config_helpers
} // namespace srsran
