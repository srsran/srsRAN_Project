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

#include "sdap_session_logger.h"
#include "srsgnb/sdap/sdap.h"

namespace srsgnb {

namespace srs_cu_up {

class sdap_entity_rx_impl : public sdap_rx_pdu_handler
{
public:
  sdap_entity_rx_impl(uint32_t ue_index, pdu_session_id_t sid, sdap_rx_sdu_notifier& sdu_notifier_) :
    logger("SDAP", {ue_index, sid, "UL"}), sdu_notifier(sdu_notifier_)
  {
  }

  void handle_pdu(byte_buffer pdu) final
  {
    // pass through
    logger.log_debug("RX SDU. sdu_len={}", pdu.length());
    sdu_notifier.on_new_sdu(std::move(pdu));
  }

private:
  sdap_session_logger   logger;
  sdap_rx_sdu_notifier& sdu_notifier;
};

} // namespace srs_cu_up

} // namespace srsgnb
