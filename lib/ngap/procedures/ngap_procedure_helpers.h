/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/ue_manager.h" // for ngap_ue
#include "srsran/ngap/ngap.h"

namespace srsran {
namespace srs_cu_cp {

inline void handle_nas_pdu(srslog::basic_logger& logger, const asn1::unbounded_octstring<true>& nas_pdu, ngap_ue& ue)
{
  logger.debug("Forwarding NAS PDU to RRC");
  byte_buffer rrc_nas_pdu;
  rrc_nas_pdu.resize(nas_pdu.size());
  std::copy(nas_pdu.begin(), nas_pdu.end(), rrc_nas_pdu.begin());
  ue.get_rrc_ue_pdu_notifier().on_new_pdu(std::move(rrc_nas_pdu));
}

} // namespace srs_cu_cp
} // namespace srsran
