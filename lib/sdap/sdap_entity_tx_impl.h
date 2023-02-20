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

#include "sdap_session_logger.h"
#include "srsran/sdap/sdap.h"

namespace srsran {

namespace srs_cu_up {

class sdap_entity_tx_impl : public sdap_tx_sdu_handler
{
public:
  sdap_entity_tx_impl(uint32_t ue_index, pdu_session_id_t sid, sdap_tx_pdu_notifier& pdu_notifier_) :
    logger("SDAP", {ue_index, sid, "DL"}), pdu_notifier(pdu_notifier_)
  {
  }

  void handle_sdu(byte_buffer sdu) final
  {
    // pass through
    logger.log_debug("TX PDU. pdu_len={}", sdu.length());
    pdu_notifier.on_new_pdu(std::move(sdu));
  }

private:
  sdap_session_logger   logger;
  sdap_tx_pdu_notifier& pdu_notifier;
};

} // namespace srs_cu_up

} // namespace srsran
