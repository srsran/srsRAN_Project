/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "sdap_session_logger.h"
#include "srsran/sdap/sdap.h"
#include "srsran/support/timers.h"

namespace srsran {

namespace srs_cu_up {

class sdap_entity_rx_impl : public sdap_rx_pdu_handler
{
public:
  sdap_entity_rx_impl(uint32_t              ue_index,
                      pdu_session_id_t      psi,
                      qos_flow_id_t         qfi_,
                      drb_id_t              drb_id_,
                      unique_timer&         ue_inactivity_timer_,
                      sdap_rx_sdu_notifier& sdu_notifier_) :
    logger("SDAP", {ue_index, psi, qfi_, drb_id_, "UL"}),
    qfi(qfi_),
    drb_id(drb_id_),
    ue_inactivity_timer(ue_inactivity_timer_),
    sdu_notifier(sdu_notifier_)
  {
  }

  void handle_pdu(byte_buffer pdu) final
  {
    // pass through with qfi
    logger.log_debug("RX SDU. {} sdu_len={}", qfi, pdu.length());
    sdu_notifier.on_new_sdu(std::move(pdu), qfi);
    ue_inactivity_timer.run();
  }

  drb_id_t get_drb_id() const { return drb_id; }

private:
  sdap_session_trx_logger logger;
  qos_flow_id_t           qfi;
  drb_id_t                drb_id;
  unique_timer&           ue_inactivity_timer;
  sdap_rx_sdu_notifier&   sdu_notifier;
};

} // namespace srs_cu_up

} // namespace srsran
