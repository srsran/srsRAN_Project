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

#include "gtpu_pdu.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/gtpu/gtpu_config.h"
#include "srsgnb/gtpu/gtpu_ul.h"
#include <cstdint>

namespace srsgnb {

/// Class used for transmitting GTP-U bearers.
class gtpu_entity_ul : public gtpu_ul_lower_layer_interface
{
public:
  gtpu_entity_ul(uint32_t ue_index, gtpu_config::gtpu_ul_config cfg_, gtpu_ul_upper_layer_notifier& upper_dn_) :
    logger(srslog::fetch_basic_logger("GTPU")), cfg(cfg_), upper_dn(upper_dn_)
  {
    // Validate configuration
    logger.info("GTPU UL entity configured.");
  }

  /*
   * SDU/PDU handlers
   */
  void handle_pdu(byte_buffer buf) final
  {
    gtpu_header hdr         = {};
    hdr.flags.version       = GTPU_FLAGS_VERSION_V1;
    hdr.flags.protocol_type = GTPU_FLAGS_GTP_PROTOCOL;
    hdr.message_type        = GTPU_MSG_DATA_PDU;
    hdr.teid                = cfg.dst_teid;
    bool write_ok           = gtpu_write_header(buf, hdr, logger);
    if (!write_ok) {
      logger.error("Error writing GTP-U header, discarding.");
      return;
    }
    upper_dn.on_new_sdu(std::move(buf));
  }

private:
  srslog::basic_logger&             logger;
  const gtpu_config::gtpu_ul_config cfg;
  gtpu_ul_upper_layer_notifier&     upper_dn;
};
} // namespace srsgnb
