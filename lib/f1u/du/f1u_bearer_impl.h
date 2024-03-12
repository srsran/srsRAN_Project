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

#include "srsran/f1u/du/f1u_bearer.h"
#include "srsran/f1u/du/f1u_bearer_logger.h"
#include "srsran/f1u/du/f1u_config.h"
#include "srsran/f1u/du/f1u_rx_sdu_notifier.h"
#include "srsran/f1u/du/f1u_tx_pdu_notifier.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace srs_du {

class f1u_bearer_impl final : public f1u_bearer,
                              public f1u_tx_sdu_handler,
                              public f1u_tx_delivery_handler,
                              public f1u_rx_pdu_handler
{
public:
  f1u_bearer_impl(uint32_t                       ue_index,
                  drb_id_t                       drb_id_,
                  const up_transport_layer_info& dl_tnl_info_,
                  const f1u_config&              config,
                  f1u_rx_sdu_notifier&           rx_sdu_notifier_,
                  f1u_tx_pdu_notifier&           tx_pdu_notifier_,
                  timer_factory                  timers,
                  task_executor&                 ue_executor_);

  f1u_tx_sdu_handler&      get_tx_sdu_handler() override { return *this; }
  f1u_tx_delivery_handler& get_tx_delivery_handler() override { return *this; }
  f1u_rx_pdu_handler&      get_rx_pdu_handler() override { return *this; }

  void handle_sdu(byte_buffer_chain sdu) override;
  void handle_transmit_notification(uint32_t highest_pdcp_sn) override;
  void handle_delivery_notification(uint32_t highest_pdcp_sn) override;
  void handle_pdu(nru_dl_message msg) override;

  void on_expired_ul_notif_timer();

private:
  f1u_bearer_logger logger;

  /// Config storage
  const f1u_config cfg;

  f1u_rx_sdu_notifier& rx_sdu_notifier;
  f1u_tx_pdu_notifier& tx_pdu_notifier;

  task_executor& ue_executor;

  /// Sentinel value representing a not-yet set PDCP SN
  static constexpr uint32_t unset_pdcp_sn = UINT32_MAX;

  /// Uplink notification timer that triggers periodic reports of highest delivered/transmitted PDCP SN towards upper
  /// layers. The purpose of this timer is to avoid excessive uplink notifications for every PDCP SN that is notified by
  /// lower layers.
  unique_timer ul_notif_timer;
  /// Holds the most recent highest transmitted PDCP SN that is frequently updated by lower layers (i.e. by RLC AM/UM)
  std::atomic<uint32_t> highest_transmitted_pdcp_sn{unset_pdcp_sn};
  /// Holds the most recent highest delivered PDCP SN that is frequently updated by lower layers (i.e. by RLC AM)
  std::atomic<uint32_t> highest_delivered_pdcp_sn{unset_pdcp_sn};
  /// Holds the last highest transmitted PDCP SN that was reported to upper layers (i.e. towards CU-UP)
  uint32_t notif_highest_transmitted_pdcp_sn = unset_pdcp_sn;
  /// Holds the last highest delivered PDCP SN that was reported to upper layers (i.e. towards CU-UP)
  uint32_t notif_highest_delivered_pdcp_sn = unset_pdcp_sn;

  bool fill_highest_transmitted_pdcp_sn(nru_dl_data_delivery_status& status);
  bool fill_highest_delivered_pdcp_sn(nru_dl_data_delivery_status& status);
  void fill_data_delivery_status(nru_ul_message& msg);

  void handle_pdu_impl(nru_dl_message msg);
};

} // namespace srs_du
} // namespace srsran
