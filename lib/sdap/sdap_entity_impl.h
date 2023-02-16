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

#include "sdap_entity_rx_impl.h"
#include "sdap_entity_tx_impl.h"
#include "srsgnb/sdap/sdap.h"
#include "srsgnb/srslog/logger.h"

namespace srsgnb {

namespace srs_cu_up {

class sdap_entity_impl : public sdap_entity
{
public:
  sdap_entity_impl(uint32_t              ue_index,
                   pdu_session_id_t      pdu_session_id,
                   sdap_tx_pdu_notifier& tx_pdu_notifier,
                   sdap_rx_sdu_notifier& rx_sdu_notifier) :
    logger(srslog::fetch_basic_logger("SDAP"))
  {
    rx = std::make_unique<sdap_entity_rx_impl>(ue_index, pdu_session_id, rx_sdu_notifier);
    tx = std::make_unique<sdap_entity_tx_impl>(ue_index, pdu_session_id, tx_pdu_notifier);
  }
  ~sdap_entity_impl() override = default;

  sdap_rx_pdu_handler& get_sdap_rx_pdu_handler() final { return *rx; };
  sdap_tx_sdu_handler& get_sdap_tx_sdu_handler() final { return *tx; };

private:
  srslog::basic_logger& logger;

  std::unique_ptr<sdap_entity_rx_impl> rx = {};
  std::unique_ptr<sdap_entity_tx_impl> tx = {};
};

} // namespace srs_cu_up

} // namespace srsgnb
