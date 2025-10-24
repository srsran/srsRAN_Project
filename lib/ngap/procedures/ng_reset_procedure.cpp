/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "ng_reset_procedure.h"
#include "ngap_asn1_converters.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

static constexpr std::chrono::milliseconds ng_reset_response_timeout{5000};

ng_reset_procedure::ng_reset_procedure(const cu_cp_reset&        msg_,
                                       ngap_message_notifier&    amf_notif_,
                                       ngap_transaction_manager& ev_mng_,
                                       ngap_ue_context_list&     ue_ctxt_list_,
                                       srslog::basic_logger&     logger_) :
  msg(msg_), amf_notifier(amf_notif_), ev_mng(ev_mng_), ue_ctxt_list(ue_ctxt_list_), logger(logger_)
{
}

void ng_reset_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" started...", name());

  // Subscribe to respective publisher to receive NG RESET ACKNOWLEDGE message.
  transaction_sink.subscribe_to(ev_mng.ng_reset_outcome, ng_reset_response_timeout);

  // Forward message to AMF.
  if (!send_ng_reset()) {
    logger.info("AMF notifier is not set. Cannot send NGReset");
    CORO_EARLY_RETURN();
  }

  // Await AMF response.
  CORO_AWAIT(transaction_sink);

  // TODO: Handle NG RESET ACKNOWLEDGE message.

  logger.debug("\"{}\" finished successfully", name());

  CORO_RETURN();
}

bool ng_reset_procedure::send_ng_reset()
{
  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_NG_RESET);
  asn1::ngap::ng_reset_s& ng_reset = ngap_msg.pdu.init_msg().value.ng_reset();
  ng_reset->cause                  = cause_to_asn1(std::get<ngap_cause_t>(msg.cause));
  if (msg.interface_reset) {
    // Reset all UEs.
    ng_reset->reset_type.set_ng_interface() = asn1::ngap::reset_all_opts::options::reset_all;
  } else {
    // Reset only specific UEs.
    ue_associated_lc_ng_conn_list_l& reset_part_of_ng_interface = ng_reset->reset_type.set_part_of_ng_interface();
    for (auto& ue : msg.ues_to_reset) {
      if (!ue_ctxt_list.contains(ue)) {
        logger.warning("ue={}: Excluding UE from NG Reset. UE context does not exist", ue);
      } else {
        auto& ue_ctxt = ue_ctxt_list[ue];

        ue_associated_lc_ng_conn_item_s conn_item;

        if (ue_ctxt.ue_ids.amf_ue_id != amf_ue_id_t::invalid) {
          conn_item.amf_ue_ngap_id_present = true;
          conn_item.amf_ue_ngap_id         = amf_ue_id_to_uint(ue_ctxt.ue_ids.amf_ue_id);
        }
        if (ue_ctxt.ue_ids.ran_ue_id != ran_ue_id_t::invalid) {
          conn_item.ran_ue_ngap_id_present = true;
          conn_item.ran_ue_ngap_id         = ran_ue_id_to_uint(ue_ctxt.ue_ids.ran_ue_id);
        }

        reset_part_of_ng_interface.push_back(conn_item);
      }
    }
  }

  // Send the message to CU-UP.
  return amf_notifier.on_new_message(ngap_msg);
}
