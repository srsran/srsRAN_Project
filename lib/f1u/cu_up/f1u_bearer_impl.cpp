/*
 *
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1u_bearer_impl.h"

using namespace srsgnb;
using namespace srs_cu_up;

f1u_bearer_impl::f1u_bearer_impl(uint32_t                  ue_index,
                                 drb_id_t                  drb_id_,
                                 f1u_tx_pdu_notifier&      tx_pdu_notifier_,
                                 f1u_rx_delivery_notifier& rx_delivery_notifier_,
                                 f1u_rx_sdu_notifier&      rx_sdu_notifier_,
                                 f1u_bearer_origin&        origin_,
                                 uint32_t                  ul_teid_) :
  logger("F1-U", {ue_index, drb_id_}),
  tx_pdu_notifier(tx_pdu_notifier_),
  rx_delivery_notifier(rx_delivery_notifier_),
  rx_sdu_notifier(rx_sdu_notifier_),
  origin(origin_),
  ul_teid(ul_teid_)
{
  (void)rx_delivery_notifier;
}

void f1u_bearer_impl::handle_pdu(nru_ul_message msg)
{
  logger.log_debug("F1-U bearer received PDU");
  // handle T-PDU
  if (!msg.t_pdu.empty()) {
    logger.log_debug("Delivering T-PDU of size={}", msg.t_pdu.length());
    rx_sdu_notifier.on_new_sdu(std::move(msg.t_pdu));
  }
  // handle transmit notifications
  if (msg.data_delivery_status.has_value()) {
    nru_dl_data_delivery_status& status = msg.data_delivery_status.value();
    // Highest successfully delivered PDCP SN
    if (status.highest_delivered_pdcp_sn.has_value()) {
      uint32_t pdcp_sn = status.highest_delivered_pdcp_sn.value();
      logger.log_debug("Notifying highest successfully delivered pdcp_sn={}", pdcp_sn);
      rx_delivery_notifier.on_delivery_notification(pdcp_sn);
    }
    // Highest transmitted PDCP SN
    if (status.highest_transmitted_pdcp_sn.has_value()) {
      uint32_t pdcp_sn = status.highest_transmitted_pdcp_sn.value();
      logger.log_debug("Notifying highest transmitted pdcp_sn={}", pdcp_sn);
      rx_delivery_notifier.on_transmit_notification(pdcp_sn);
    }
    // Highest successfully delivered retransmitted PDCP SN
    if (status.highest_delivered_retransmitted_pdcp_sn.has_value()) {
      uint32_t pdcp_sn = status.highest_delivered_retransmitted_pdcp_sn.value();
      logger.log_warning("Unhandled highest successfully delivered retransmitted pdcp_sn={}", pdcp_sn);
      // TODO
    }
    // Highest retransmitted PDCP SN
    if (status.highest_retransmitted_pdcp_sn.has_value()) {
      uint32_t pdcp_sn = status.highest_retransmitted_pdcp_sn.value();
      logger.log_warning("Unhandled highest retransmitted pdcp_sn={}", pdcp_sn);
      // TODO
    }
  }
}

void f1u_bearer_impl::handle_sdu(pdcp_tx_pdu sdu)
{
  logger.log_debug("F1-U bearer received SDU with pdcp_sn={}, size={}", sdu.pdcp_sn, sdu.buf.length());
  nru_dl_message msg = {};
  msg.t_pdu          = std::move(sdu.buf);
  msg.pdcp_sn        = sdu.pdcp_sn;
  tx_pdu_notifier.on_new_pdu(std::move(msg));
}

void f1u_bearer_impl::discard_sdu(uint32_t pdcp_sn)
{
  logger.log_debug("F1-U bearer received order to discard SDU with pdcp_sn={}", pdcp_sn);
  nru_dl_message msg              = {};
  msg.dl_user_data.discard_blocks = nru_pdcp_sn_discard_blocks{};
  nru_pdcp_sn_discard_block block = {};
  block.pdcp_sn_start             = pdcp_sn;
  block.block_size                = 1;
  msg.dl_user_data.discard_blocks.value().push_back(std::move(block));
  tx_pdu_notifier.on_new_pdu(std::move(msg));
}
