/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "cu_up_unit_logger_config.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

/// Registers the CU-UP loggers in the logger service.
inline void register_cu_up_loggers(const cu_up_unit_logger_config& log_cfg)
{
  for (const auto& id : {"CU-UP", "CU-UP-E1"}) {
    auto& cu_up_logger = srslog::fetch_basic_logger(id, false);
    cu_up_logger.set_level(log_cfg.cu_level);
    cu_up_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& pdcp_logger = srslog::fetch_basic_logger("PDCP", false);
  pdcp_logger.set_level(log_cfg.pdcp_level);
  pdcp_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& f1u_logger = srslog::fetch_basic_logger("CU-F1-U", false);
  f1u_logger.set_level(log_cfg.f1u_level);
  f1u_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& gtpu_logger = srslog::fetch_basic_logger("GTPU", false);
  gtpu_logger.set_level(log_cfg.gtpu_level);
  gtpu_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& sdap_logger = srslog::fetch_basic_logger("SDAP", false);
  sdap_logger.set_level(log_cfg.sdap_level);
  sdap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
}

} // namespace srsran
