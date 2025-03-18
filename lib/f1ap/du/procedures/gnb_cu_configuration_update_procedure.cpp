/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  CORO_AWAIT(du_mng.request_cu_context_update(request_du_update()));

  send_gnb_cu_configuration_update_acknowledge();

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

  return du_req;
}

void gnb_cu_configuration_update_procedure::send_gnb_cu_configuration_update_acknowledge()
{
  using namespace asn1::f1ap;

  f1ap_message msg = {};

  msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_GNB_CU_CFG_UPD);
  gnb_cu_cfg_upd_ack_s& ack = msg.pdu.successful_outcome().value.gnb_cu_cfg_upd_ack();
  ack->transaction_id       = request->transaction_id;

  // send F1AP message.
  cu_notif.on_new_message(msg);
}
