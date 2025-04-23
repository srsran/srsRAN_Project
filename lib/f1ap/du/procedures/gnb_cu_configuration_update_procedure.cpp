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

#include "gnb_cu_configuration_update_procedure.h"
#include "../../asn1_helpers.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/f1ap/f1ap_message_notifier.h"

using namespace srsran;
using namespace srsran::srs_du;

gnb_cu_configuration_update_procedure::gnb_cu_configuration_update_procedure(const asn1::f1ap::gnb_cu_cfg_upd_s& msg_,
                                                                             f1ap_du_configurator&  du_mng_,
                                                                             f1ap_message_notifier& cu_notif_) :
  request(msg_), du_mng(du_mng_), cu_notif(cu_notif_)
{
}

void gnb_cu_configuration_update_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  CORO_AWAIT_VALUE(du_resp, du_mng.request_cu_context_update(request_du_update()));

  send_response();

  CORO_RETURN();
}

gnbcu_config_update_request gnb_cu_configuration_update_procedure::request_du_update()
{
  gnbcu_config_update_request du_req;

  if (request->cells_to_be_activ_list_present) {
    for (const auto& item : request->cells_to_be_activ_list) {
      auto& asn1_cell = item->cells_to_be_activ_list_item();
      auto  cgi       = cgi_from_asn1(asn1_cell.nr_cgi);
      if (cgi.has_value()) {
        du_req.cells_to_activate.push_back({cgi.value()});
      }
    }
  }

  if (request->cells_to_be_deactiv_list_present) {
    for (const auto& item : request->cells_to_be_deactiv_list) {
      auto& asn1_cell = item->cells_to_be_deactiv_list_item();
      auto  cgi       = cgi_from_asn1(asn1_cell.nr_cgi);
      if (cgi.has_value()) {
        du_req.cells_to_deactivate.push_back(cgi.value());
      }
    }
  }

  return du_req;
}

void gnb_cu_configuration_update_procedure::send_response()
{
  using namespace asn1::f1ap;

  f1ap_message msg;

  bool is_ack = true;

  if (not du_resp.cells_failed_to_activate.empty()) {
    if (du_resp.cells_failed_to_activate.size() == request->cells_to_be_activ_list.size()) {
      is_ack = false;
    }
  }

  if (is_ack) {
    msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_GNB_CU_CFG_UPD);
    gnb_cu_cfg_upd_ack_s& ack = msg.pdu.successful_outcome().value.gnb_cu_cfg_upd_ack();
    ack->transaction_id       = request->transaction_id;

    // Add cells failed to be activated.
    ack->cells_failed_to_be_activ_list.resize(du_resp.cells_failed_to_activate.size());
    for (unsigned i = 0, e = du_resp.cells_failed_to_activate.size(); i != e; ++i) {
      ack->cells_failed_to_be_activ_list[i].load_info_obj(ASN1_F1AP_ID_CELLS_FAILED_TO_BE_ACTIV_LIST_ITEM);
      ack->cells_failed_to_be_activ_list[i]->cells_failed_to_be_activ_list_item().nr_cgi =
          cgi_to_asn1(du_resp.cells_failed_to_activate[i]);
      ack->cells_failed_to_be_activ_list[i]->cells_failed_to_be_activ_list_item().cause.set_misc();
      ack->cells_failed_to_be_activ_list[i]->cells_failed_to_be_activ_list_item().cause.misc().value =
          cause_misc_opts::unspecified;
    }
    ack->cells_failed_to_be_activ_list_present = ack->cells_failed_to_be_activ_list.size() > 0;

  } else {
    msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_GNB_CU_CFG_UPD);
    gnb_cu_cfg_upd_fail_s& fail  = msg.pdu.unsuccessful_outcome().value.gnb_cu_cfg_upd_fail();
    fail->transaction_id         = request->transaction_id;
    fail->cause.set_misc().value = cause_misc_opts::unspecified;
  }

  // send F1AP message.
  cu_notif.on_new_message(msg);
}
