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
#include "../../../ran/gnb_format.h"
#include "du/procedures/f1ap_du_event_manager.h"

using namespace srsgnb::srs_du;

f1c_srb0_du_bearer::f1c_srb0_du_bearer(f1c_ue_context&            ue_ctxt_,
                                       const asn1::f1ap::nrcgi_s& nr_cgi_,
                                       const byte_buffer&         du_cu_rrc_container_,
                                       f1c_message_notifier&      f1c_notifier_,
                                       f1_tx_pdu_notifier&        f1c_pdu_notifier_,
                                       f1ap_event_manager&        ev_manager_) :
  ue_ctxt(ue_ctxt_),
  nr_cgi(nr_cgi_),
  du_cu_rrc_container(du_cu_rrc_container_.copy()),
  f1c_notifier(f1c_notifier_),
  pdu_notifier(f1c_pdu_notifier_),
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
  init_msg->gnb_du_ue_f1_ap_id->value             = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.gnb_du_ue_f1ap_id);
  init_msg->nrcgi.value                           = nr_cgi;
  init_msg->c_rnti->value                         = ue_ctxt.rnti;
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

  log_ue_event(logger,
               ue_event_prefix{"UL", ue_ctxt.ue_index}.set_channel("SRB0") | ue_ctxt.rnti,
               "Initial UL RRC Message Transfer.");
}

void f1c_srb0_du_bearer::handle_sdu(byte_buffer sdu)
{
  pdu_notifier.on_tx_pdu(std::move(sdu));

  log_ue_event(
      logger, ue_event_prefix{"DL", ue_ctxt.ue_index}.set_channel("SRB0") | ue_ctxt.rnti, "DL RRC Message Transfer.");
}

f1c_other_srb_du_bearer::f1c_other_srb_du_bearer(f1c_ue_context&       ue_ctxt_,
                                                 srb_id_t              srb_id_,
                                                 f1c_message_notifier& f1c_notifier_,
                                                 f1_tx_pdu_notifier&   f1c_pdu_notifier_) :
  ue_ctxt(ue_ctxt_),
  srb_id(srb_id_),
  f1c_notifier(f1c_notifier_),
  pdu_notifier(f1c_pdu_notifier_),
  logger(srslog::fetch_basic_logger("F1AP-DU"))
{
}

void f1c_other_srb_du_bearer::handle_pdu(byte_buffer_slice_chain pdu)
{
  f1c_message msg;

  // Fill F1AP UL RRC Message Transfer.
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_ULRRC_MSG_TRANSFER);
  asn1::f1ap::ulrrc_msg_transfer_s& ul_msg = msg.pdu.init_msg().value.ulrrc_msg_transfer();
  ul_msg->gnb_du_ue_f1_ap_id->value        = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.gnb_du_ue_f1ap_id);
  ul_msg->gnb_cu_ue_f1_ap_id->value        = gnb_cu_ue_f1ap_id_to_uint(ue_ctxt.gnb_cu_ue_f1ap_id);
  ul_msg->srbid->value                     = srb_id_to_uint(srb_id);
  ul_msg->rrc_container->resize(pdu.length());
  std::copy(pdu.begin(), pdu.end(), ul_msg->rrc_container->begin());
  ul_msg->sel_plmnid_present              = false;
  ul_msg->new_g_nb_du_ue_f1_ap_id_present = false;

  f1c_notifier.on_new_message(msg);

  log_ue_event(logger,
               ue_event_prefix{"UL", ue_ctxt.ue_index}.set_channel(srb_id_to_string(srb_id)) | ue_ctxt.rnti,
               "UL RRC Message Transfer.");
}

void f1c_other_srb_du_bearer::handle_sdu(srsgnb::byte_buffer sdu)
{
  pdu_notifier.on_tx_pdu(std::move(sdu));

  log_ue_event(logger,
               ue_event_prefix{"DL", ue_ctxt.ue_index}.set_channel(srb_id_to_string(srb_id)) | ue_ctxt.rnti,
               "DL RRC Message Transfer.");
}