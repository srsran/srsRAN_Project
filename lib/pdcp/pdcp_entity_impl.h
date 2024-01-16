/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "pdcp_entity_rx.h"
#include "pdcp_entity_tx.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/pdcp/pdcp_entity.h"
#include <cstdio>
#include <memory>

namespace srsran {

class pdcp_entity_impl : public pdcp_entity
{
public:
  pdcp_entity_impl(uint32_t                        ue_index,
                   rb_id_t                         rb_id,
                   pdcp_config                     config,
                   pdcp_tx_lower_notifier&         tx_lower_dn,
                   pdcp_tx_upper_control_notifier& tx_upper_cn,
                   pdcp_rx_upper_data_notifier&    rx_upper_dn,
                   pdcp_rx_upper_control_notifier& rx_upper_cn,
                   timer_factory                   timers)
  {
    tx = std::make_unique<pdcp_entity_tx>(ue_index, rb_id, config.get_tx_config(), tx_lower_dn, tx_upper_cn, timers);
    rx = std::make_unique<pdcp_entity_rx>(ue_index, rb_id, config.get_rx_config(), rx_upper_dn, rx_upper_cn, timers);

    // Tx/Rx interconnect
    tx->set_status_provider(rx.get());
    rx->set_status_handler(tx.get());
  }
  ~pdcp_entity_impl() override = default;
  pdcp_tx_upper_control_interface& get_tx_upper_control_interface() final { return (*tx); };
  pdcp_tx_upper_data_interface&    get_tx_upper_data_interface() final { return (*tx); };
  pdcp_tx_lower_interface&         get_tx_lower_interface() final { return (*tx); };
  pdcp_rx_upper_control_interface& get_rx_upper_control_interface() final { return (*rx); };
  pdcp_rx_lower_interface&         get_rx_lower_interface() final { return (*rx); };

  pdcp_metrics_container get_metrics() final
  {
    pdcp_metrics_container m;
    m.tx = tx->get_metrics();
    m.rx = rx->get_metrics();
    return m;
  };

private:
  std::unique_ptr<pdcp_entity_tx> tx = {};
  std::unique_ptr<pdcp_entity_rx> rx = {};
};
} // namespace srsran
