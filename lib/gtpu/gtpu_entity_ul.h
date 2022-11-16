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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/gtpu/gtpu_ul.h"
#include <cstdint>

namespace srsgnb {

/// Base class used for transmitting PDCP bearers.
/// It provides interfaces for the PDCP bearers, for the higher and lower layers
class gtpu_entity_ul : public gtpu_ul_lower_layer_interface
{
public:
  gtpu_entity_ul(uint32_t ue_index) : logger(srslog::fetch_basic_logger("GTPU"))
  {
    // Validate configuration
    logger.info("GTPU UL entity configured.");
  }

  /*
   * SDU/PDU handlers
   */
  void handle_pdu(byte_buffer sdu) final;

private:
  srslog::basic_logger& logger;
};
} // namespace srsgnb
