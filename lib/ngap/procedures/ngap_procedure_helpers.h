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

#include "../ue_context/ngap_ue_logger.h"
#include "srsran/ngap/ngap.h"

namespace srsran {
namespace srs_cu_cp {

inline void handle_nas_pdu(ngap_ue_logger& logger, byte_buffer nas_pdu, ngap_rrc_ue_pdu_notifier& rrc_ue_pdu_notifier)
{
  logger.log_debug("Forwarding NAS PDU to RRC");
  rrc_ue_pdu_notifier.on_new_pdu(std::move(nas_pdu));
}

} // namespace srs_cu_cp
} // namespace srsran
