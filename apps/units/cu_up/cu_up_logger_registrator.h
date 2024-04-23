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

#include "cu_up_unit_logger_config.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

/// Registers the CU-UP loggers in the logger service.
inline void register_cu_up_loggers(const cu_up_unit_logger_config& log_cfg)
{
  for (const auto& id : {"CU-UP", "CU-UP-E1"}) {
    auto& cu_up_logger = srslog::fetch_basic_logger(id, false);
    cu_up_logger.set_level(srslog::str_to_basic_level(log_cfg.cu_level));
    cu_up_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& pdcp_logger = srslog::fetch_basic_logger("PDCP", false);
  pdcp_logger.set_level(srslog::str_to_basic_level(log_cfg.pdcp_level));
  pdcp_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& f1u_logger = srslog::fetch_basic_logger("CU-F1-U", false);
  f1u_logger.set_level(srslog::str_to_basic_level(log_cfg.f1u_level));
  f1u_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& gtpu_logger = srslog::fetch_basic_logger("GTPU", false);
  gtpu_logger.set_level(srslog::str_to_basic_level(log_cfg.gtpu_level));
  gtpu_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& sdap_logger = srslog::fetch_basic_logger("SDAP", false);
  sdap_logger.set_level(srslog::str_to_basic_level(log_cfg.sdap_level));
  sdap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
}

} // namespace srsran
