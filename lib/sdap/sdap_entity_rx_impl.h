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
#include "srsran/support/timers.h"

namespace srsran {

namespace srs_cu_up {

class sdap_entity_rx_impl : public sdap_rx_pdu_handler
{
public:
  sdap_entity_rx_impl(uint32_t                ue_index,
                      pdu_session_id_t        sid,
                      optional<qos_flow_id_t> qfi_,
                      drb_id_t                drb_id_,
                      unique_timer&           ue_inactivity_timer_,
                      sdap_rx_sdu_notifier&   sdu_notifier_) :
    logger("SDAP", {ue_index, sid, qfi_, drb_id_, "UL"}),
    qfi(qfi_),
    drb_id(drb_id_),
    ue_inactivity_timer(ue_inactivity_timer_),
    sdu_notifier(sdu_notifier_)
  {
  }

  void handle_pdu(byte_buffer pdu) final
  {
    // pass through
    logger.log_debug("RX SDU. {} sdu_len={}", qfi, pdu.length());
    sdu_notifier.on_new_sdu(std::move(pdu));
    ue_inactivity_timer.run();
  }

  drb_id_t get_drb_id() const { return drb_id; }

private:
  sdap_session_trx_logger logger;
  optional<qos_flow_id_t> qfi;
  drb_id_t                drb_id;
  unique_timer&           ue_inactivity_timer;
  sdap_rx_sdu_notifier&   sdu_notifier;
};

} // namespace srs_cu_up

} // namespace srsran
