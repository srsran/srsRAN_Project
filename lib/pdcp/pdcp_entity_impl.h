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

#include "pdcp_entity_rx.h"
#include "pdcp_entity_tx.h"
#include "srsgnb/pdcp/pdcp_entity.h"
#include "srsgnb/ran/bearer_logger.h"
#include <cstdio>

namespace srsgnb {

class pdcp_entity_impl : public pdcp_entity
{
public:
  pdcp_entity_impl(pdcp_tx_lower_notifier&         tx_lower_dn,
                   pdcp_tx_upper_control_notifier& tx_upper_cn,
                   pdcp_rx_upper_data_notifier&    rx_upper_dn)
  {
    tx = std::unique_ptr<pdcp_entity_tx>(new pdcp_entity_tx(tx_lower_dn, tx_upper_cn));
    rx = std::unique_ptr<pdcp_entity_rx>(new pdcp_entity_rx(rx_upper_dn));
  }
  ~pdcp_entity_impl() override = default;
  pdcp_tx_upper_data_interface& get_tx_upper_data_interface() final { return (*tx); };
  pdcp_tx_lower_interface&      get_tx_lower_interface() final { return (*tx); };
  pdcp_rx_lower_interface&      get_rx_lower_interface() final { return (*rx); };

private:
  std::unique_ptr<pdcp_entity_tx> tx = {};
  std::unique_ptr<pdcp_entity_rx> rx = {};
};
} // namespace srsgnb
