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

#include "cu_cp_log_unit_config.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

/// Registers the CU-CP loggers in the logger service.
inline void register_logs(const cu_cp_log_unit_config& log_cfg)
{
  for (const auto& id : {"CU-CP", "CU-UEMNG", "CU-CP-E1"}) {
    auto& cu_cp_logger = srslog::fetch_basic_logger(id, false);
    cu_cp_logger.set_level(srslog::str_to_basic_level(log_cfg.cu_level));
    cu_cp_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& pdcp_logger = srslog::fetch_basic_logger("PDCP", false);
  pdcp_logger.set_level(srslog::str_to_basic_level(log_cfg.pdcp_level));
  pdcp_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& rrc_logger = srslog::fetch_basic_logger("RRC", false);
  rrc_logger.set_level(srslog::str_to_basic_level(log_cfg.rrc_level));
  rrc_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& cu_f1ap_logger = srslog::fetch_basic_logger("CU-CP-F1", false);
  cu_f1ap_logger.set_level(srslog::str_to_basic_level(log_cfg.f1ap_level));
  cu_f1ap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& ngap_logger = srslog::fetch_basic_logger("NGAP", false);
  ngap_logger.set_level(srslog::str_to_basic_level(log_cfg.ngap_level));
  ngap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& sec_logger = srslog::fetch_basic_logger("SEC", false);
  sec_logger.set_level(srslog::str_to_basic_level(log_cfg.sec_level));
  sec_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
}

} // namespace srsran
