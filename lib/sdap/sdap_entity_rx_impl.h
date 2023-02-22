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

namespace srsran {

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

} // namespace srsran
