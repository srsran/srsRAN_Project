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

#include "srsran/du_manager/du_manager.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/mac/mac.h"
#include "srsran/mac/mac_cell_control_information_handler.h"
#include "srsran/rlc/rlc_rx.h"
#include "srsran/rlc/rlc_tx.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace srs_du {

class du_manager_mac_event_indicator : public mac_ul_ccch_notifier
{
public:
  void connect(du_manager_mac_event_handler& du_mng_) { du_mng = &du_mng_; }

  void on_ul_ccch_msg_received(const ul_ccch_indication_message& msg) override
  {
    du_mng->handle_ul_ccch_indication(msg);
  }

private:
  du_manager_mac_event_handler* du_mng;
};

class mac_f1ap_paging_handler : public f1ap_du_paging_notifier
{
public:
  void connect(mac_paging_information_handler& mac_) { mac = &mac_; }

  void on_paging_received(const paging_information& msg) override { mac->handle_paging_information(msg); }

private:
  mac_paging_information_handler* mac;
};

} // namespace srs_du
} // namespace srsran
