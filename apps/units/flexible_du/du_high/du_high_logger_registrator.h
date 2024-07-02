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

#include "du_high_config.h"

namespace srsran {

/// Registers the DU high loggers in the logger service.
inline void register_du_high_loggers(const du_high_unit_logger_config& log_cfg)
{
  for (const auto& id : {"DU", "DU-MNG", "UE-MNG"}) {
    auto& du_logger = srslog::fetch_basic_logger(id, false);
    du_logger.set_level(log_cfg.du_level);
    du_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  for (const auto& id : {"MAC", "SCHED"}) {
    auto& mac_logger = srslog::fetch_basic_logger(id, true);
    mac_logger.set_level(log_cfg.mac_level);
    mac_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& rlc_logger = srslog::fetch_basic_logger("RLC", false);
  rlc_logger.set_level(log_cfg.rlc_level);
  rlc_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& du_f1ap_logger = srslog::fetch_basic_logger("DU-F1", false);
  du_f1ap_logger.set_level(log_cfg.f1ap_level);
  du_f1ap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& f1u_logger = srslog::fetch_basic_logger("DU-F1-U", false);
  f1u_logger.set_level(log_cfg.f1u_level);
  f1u_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
}

} // namespace srsran
