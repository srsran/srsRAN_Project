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

  span<const uint8_t> encode_rar_pdu(const rar_information& rar);

private:
  const mac_cell_creation_request& cell_cfg;

  unsigned                          next_index = 0;
  std::vector<std::vector<uint8_t>> rar_payload_ring_buffer;
};

} // namespace srsgnb
