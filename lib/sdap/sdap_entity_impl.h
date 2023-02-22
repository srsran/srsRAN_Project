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

namespace srsran {

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

} // namespace srsran
