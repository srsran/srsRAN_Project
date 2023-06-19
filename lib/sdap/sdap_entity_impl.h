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

#include "sdap_entity_rx_impl.h"
#include "sdap_entity_tx_impl.h"
#include "srsran/sdap/sdap.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/timers.h"
#include <unordered_map>

namespace srsran {

namespace srs_cu_up {

class sdap_entity_impl : public sdap_entity, public sdap_tx_sdu_handler
{
public:
  sdap_entity_impl(uint32_t              ue_index_,
                   pdu_session_id_t      pdu_session_id_,
                   unique_timer&         ue_inactivity_timer_,
                   sdap_rx_sdu_notifier& rx_sdu_notifier_) :
    logger(srslog::fetch_basic_logger("SDAP")),
    ue_index(ue_index_),
    pdu_session_id(pdu_session_id_),
    ue_inactivity_timer(ue_inactivity_timer_),
    rx_sdu_notifier(rx_sdu_notifier_)
  {
  }
  ~sdap_entity_impl() override = default;

  sdap_rx_pdu_handler& get_sdap_rx_pdu_handler(drb_id_t drb_id) final
  {
    auto rx_it = rx_map.find(drb_id);
    srsran_assert(rx_it != rx_map.end(), "Cannot find QFI mapping for {} in PDU session {}", drb_id, pdu_session_id);
    return *rx_it->second;
  };
  sdap_tx_sdu_handler& get_sdap_tx_sdu_handler() final { return *this; };

  /// Handle the incoming SDU and redirect to mapped DRB.
  void handle_sdu(byte_buffer sdu, qos_flow_id_t qos_flow_id) final
  {
    auto tx_it = tx_map.find(qos_flow_id);
    if (tx_it == tx_map.end()) {
      logger.warning("No mapping for SDU with qfi={} in PDU session {}", qos_flow_id, pdu_session_id);
      return;
    }
    tx_it->second->handle_sdu(std::move(sdu));
  }

  void
  add_mapping(qos_flow_id_t qfi, drb_id_t drb_id, sdap_config sdap_cfg, sdap_tx_pdu_notifier& tx_pdu_notifier) final
  {
    // check preconditions
    if (tx_map.find(qfi) != tx_map.end()) {
      logger.error("Cannot overwrite existing DL mapping for qfi={} in PDU session {}", qfi, pdu_session_id);
      return;
    }
    if (rx_map.find(drb_id) != rx_map.end()) {
      logger.error("Cannot overwrite existing UL mapping for {} in PDU session {}", drb_id, pdu_session_id);
      return;
    }

    // create TX mapping
    std::unique_ptr<sdap_entity_tx_impl> tx = std::make_unique<sdap_entity_tx_impl>(
        ue_index, pdu_session_id, qfi, drb_id, ue_inactivity_timer, tx_pdu_notifier);
    tx_map.insert({qfi, std::move(tx)});

    // create RX mapping
    std::unique_ptr<sdap_entity_rx_impl> rx = std::make_unique<sdap_entity_rx_impl>(
        ue_index, pdu_session_id, qfi, drb_id, ue_inactivity_timer, rx_sdu_notifier);
    rx_map.insert({drb_id, std::move(rx)});
  }

  void remove_mapping(drb_id_t drb_id) final
  {
    // remove TX mapping
    for (auto tx_it = tx_map.begin(); tx_it != tx_map.end();) {
      if (tx_it->second->get_drb_id() == drb_id) {
        tx_it = tx_map.erase(tx_it);
      } else {
        ++tx_it;
      }
    }

    // remove RX mapping
    rx_map.erase(drb_id);
  }

private:
  srslog::basic_logger& logger;
  uint32_t              ue_index;
  pdu_session_id_t      pdu_session_id;
  unique_timer&         ue_inactivity_timer;
  sdap_rx_sdu_notifier& rx_sdu_notifier;

  std::unordered_map<qos_flow_id_t, std::unique_ptr<sdap_entity_tx_impl>> tx_map;
  std::unordered_map<drb_id_t, std::unique_ptr<sdap_entity_rx_impl>>      rx_map;
};

} // namespace srs_cu_up

} // namespace srsran
