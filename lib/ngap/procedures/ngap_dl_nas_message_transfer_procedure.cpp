/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_dl_nas_message_transfer_procedure.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_dl_nas_message_transfer_procedure::ngap_dl_nas_message_transfer_procedure(
    const ngap_dl_nas_transport_message&         msg_,
    ngap_rrc_ue_pdu_notifier&                    rrc_ue_pdu_notifier_,
    ngap_rrc_ue_control_notifier&                rrc_ue_ctrl_notifier_,
    ngap_ue_radio_capability_management_handler& ngap_handler_,
    ngap_ue_logger&                              logger_) :
  msg(msg_),
  rrc_ue_pdu_notifier(rrc_ue_pdu_notifier_),
  rrc_ue_ctrl_notifier(rrc_ue_ctrl_notifier_),
  ngap_handler(ngap_handler_),
  logger(logger_)
{
}

void ngap_dl_nas_message_transfer_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" initialized", name());

  send_pdu_to_rrc_ue();

  if (msg.ue_cap_info_request) {
    // Trigger UE Radio Capability Info Indication
    logger.log_info("UE Radio Capability Information requested");
    send_ue_radio_capability_info_indication();
  }

  logger.log_debug("\"{}\" finalized", name());

  CORO_RETURN();
}

void ngap_dl_nas_message_transfer_procedure::send_pdu_to_rrc_ue()
{
  rrc_ue_pdu_notifier.on_new_pdu(std::move(msg.nas_pdu));
}

void ngap_dl_nas_message_transfer_procedure::send_ue_radio_capability_info_indication()
{
  ngap_ue_radio_capability_info_indication ue_radio_cap_info_indication;
  ue_radio_cap_info_indication.ue_index                  = msg.ue_index;
  ue_radio_cap_info_indication.ue_cap_rat_container_list = rrc_ue_ctrl_notifier.on_ue_radio_access_cap_info_required();
  // Only transmit UE Radio Capability Info Indication if UE Capabilities are available
  if (!ue_radio_cap_info_indication.ue_cap_rat_container_list.empty()) {
    ngap_handler.handle_tx_ue_radio_capability_info_indication_required(ue_radio_cap_info_indication);
  }
}
