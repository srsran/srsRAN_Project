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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace srs_cu_cp {

class du_processor_repository;
struct cu_cp_paging_message;

/// Class responsible for handling incoming paging messages and forwarding them to the appropriate DUs.
class paging_message_handler
{
public:
  paging_message_handler(du_processor_repository& dus_);

  /// Handle Paging message sent by the core network and distribute across the served DU cells.
  void handle_paging_message(const cu_cp_paging_message& msg);

private:
  bool handle_du_paging_message(du_index_t du_index, const cu_cp_paging_message& msg);

  du_processor_repository& dus;
  srslog::basic_logger&    logger;
};

} // namespace srs_cu_cp
} // namespace srsran
