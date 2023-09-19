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

#include "srsran/f1u/cu_up/f1u_bearer.h"
#include "srsran/f1u/cu_up/f1u_bearer_logger.h"
#include "srsran/f1u/cu_up/f1u_rx_delivery_notifier.h"
#include "srsran/f1u/cu_up/f1u_rx_sdu_notifier.h"
#include "srsran/f1u/cu_up/f1u_tx_pdu_notifier.h"
#include "srsran/ran/lcid.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace srs_cu_up {

class f1u_bearer_impl final : public f1u_bearer, public f1u_rx_pdu_handler, public f1u_tx_sdu_handler
{
public:
  f1u_bearer_impl(uint32_t                       ue_index,
                  drb_id_t                       drb_id_,
                  const up_transport_layer_info& ul_tnl_info_,
                  f1u_tx_pdu_notifier&           tx_pdu_notifier_,
                  f1u_rx_delivery_notifier&      rx_delivery_notifier_,
                  f1u_rx_sdu_notifier&           rx_sdu_notifier_,
                  timer_factory                  timers,
                  f1u_bearer_disconnector&       diconnector_);
  f1u_bearer_impl(const f1u_bearer_impl&)            = delete;
  f1u_bearer_impl& operator=(const f1u_bearer_impl&) = delete;

  ~f1u_bearer_impl() override { disconnector.disconnect_cu_bearer(ul_tnl_info); }

  f1u_rx_pdu_handler& get_rx_pdu_handler() override { return *this; }
  f1u_tx_sdu_handler& get_tx_sdu_handler() override { return *this; }

  void handle_pdu(nru_ul_message msg) override;
  void handle_sdu(pdcp_tx_pdu sdu) override;
  void discard_sdu(uint32_t pdcp_sn) override;

  /// \brief Returns the UL tunnel info that was assigned upon construction.
  const up_transport_layer_info& get_ul_tnl_info() const { return ul_tnl_info; }

  /// \brief Returns the UL TEID that was assigned upon construction.
  /// \return The UL TEID associated with this bearer.
  gtpu_teid_t get_ul_teid() const { return ul_tnl_info.gtp_teid; }

  /// \brief This function handles the periodic transmission of downlink notification towards lower layers with all
  /// discard blocks that have aggregated since the previous DL PDU.
  void on_expired_dl_notif_timer();

private:
  f1u_bearer_logger         logger;
  f1u_tx_pdu_notifier&      tx_pdu_notifier;
  f1u_rx_delivery_notifier& rx_delivery_notifier;
  f1u_rx_sdu_notifier&      rx_sdu_notifier;
  f1u_bearer_disconnector&  disconnector;
  up_transport_layer_info   ul_tnl_info;

  /// Downlink notification timer that triggers periodic transmission of discard blocks towards lower layers. The
  /// purpose of this timer is to avoid excessive downlink notifications for every PDCP SN that is discarded by upper
  /// layers.
  unique_timer dl_notif_timer;

  /// Collection of pending \c nru_pdcp_sn_discard_block objects.
  nru_pdcp_sn_discard_blocks discard_blocks;

  /// \brief Fills the provided \c nru_dl_message with all SDU discard blocks that have been aggregated since the last
  /// call of this function (or since creation of this object).
  /// \param msg The message to be filled with SDU discard blocks.
  void fill_discard_blocks(nru_dl_message& msg);

  /// \brief Immediately transmits a \c nru_dl_message with all currently aggregated SDU discard blocks, if any.
  void flush_discard_blocks();
};

} // namespace srs_cu_up
} // namespace srsran
