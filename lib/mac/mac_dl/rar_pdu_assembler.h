/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/mac/cell_configuration.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

class rar_pdu_assembler
{
public:
  explicit rar_pdu_assembler(const mac_cell_creation_request& cell_cfg_);

  span<const uint8_t> encode_rar_pdu(slot_point sl_tx, const rar_information& rar);

private:
  struct slot_resources {
    unsigned                                     next_idx = 0;
    std::array<std::vector<uint8_t>, MAX_GRANTS> payloads;
  };

  const mac_cell_creation_request& cell_cfg;

  slot_point                  last_sl_tx;
  std::vector<slot_resources> rar_payload_grid;
};

} // namespace srsgnb
