/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1c_du_bearer_impl.h"
#include "../../ran/gnb_format.h"
#include "procedures/f1ap_du_event_manager.h"

using namespace srsgnb::srs_du;

f1c_srb0_du_bearer::f1c_srb0_du_bearer(du_ue_index_t              ue_index_,
                                       gnb_du_ue_f1ap_id_t        du_ue_id,
                                       rnti_t                     c_rnti_,
                                       const asn1::f1ap::nrcgi_s& nr_cgi_,
                                       byte_buffer                du_cu_rrc_container_,
                                       f1c_message_notifier&      f1c_notifier_,
                                       f1ap_event_manager&        ev_manager_) :
  ue_index(ue_index_),
  gnb_du_f1ap_ue_id(du_ue_id),
  c_rnti(c_rnti_),
  nr_cgi(nr_cgi_),
  du_cu_rrc_container(std::move(du_cu_rrc_container_)),
  f1c_notifier(f1c_notifier_),
  ev_manager(ev_manager_),
  logger(srslog::fetch_basic_logger("F1AP-DU"))
{
}

void f1c_srb0_du_bearer::handle_pdu(byte_buffer_slice_chain pdu)
{
  protocol_transaction<f1ap_outcome> transaction = ev_manager.transactions.create_transaction();

  // Pack Initial UL RRC Message Transfer as per TS38.473, Section 8.4.1.
  f1c_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_INIT_ULRRC_MSG_TRANSFER);
  asn1::f1ap::init_ulrrc_msg_transfer_s& init_msg = msg.pdu.init_msg().value.init_ulrrc_msg_transfer();
  init_msg->gnb_du_ue_f1_ap_id->value             = gnb_du_ue_f1ap_id_to_uint(gnb_du_f1ap_ue_id);
  init_msg->nrcgi.value                           = nr_cgi;
  init_msg->c_rnti->value                         = c_rnti;
  init_msg->rrc_container.value.resize(pdu.length());
  std::copy(pdu.begin(), pdu.end(), init_msg->rrc_container->begin());
  init_msg->duto_currc_container_present = true;
  init_msg->duto_currc_container->resize(du_cu_rrc_container.length());
  std::copy(du_cu_rrc_container.begin(), du_cu_rrc_container.end(), init_msg->duto_currc_container->begin());
  init_msg->sul_access_ind_present                   = false;
  init_msg->transaction_id->value                    = transaction.id();
  init_msg->ranueid_present                          = false;
  init_msg->rrc_container_rrc_setup_complete_present = false;

  // Notify upper layers of the initial UL RRC Message Transfer.
  f1c_notifier.on_new_message(msg);

  // Signal that the transaction has completed and the DU does not expect a response.
  ev_manager.transactions.set(transaction.id(), f1ap_outcome{});

  log_ue_event(
      logger, ue_event_prefix{"UL", ue_index}.set_channel("SRB0") | c_rnti, "Initial UL RRC Message Transfer.");
}
