/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/f1_interface/common/f1c_types.h"
#include "srsgnb/f1_interface/du/f1_du_bearer.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/ran/rnti.h"

namespace srsgnb {
namespace srs_du {

class f1ap_event_manager;

class f1c_srb0_du_bearer final : public f1_du_bearer
{
public:
  f1c_srb0_du_bearer(du_ue_index_t              ue_index_,
                     gnb_du_ue_f1ap_id_t        du_ue_id,
                     rnti_t                     c_rnti_,
                     const asn1::f1ap::nrcgi_s& nr_cgi_,
                     const byte_buffer&         du_cu_rrc_container_,
                     f1c_message_notifier&      f1c_notifier_,
                     f1ap_event_manager&        ev_manager_);

  /// \brief Packs and forwards the initial UL RRC message transfer as per TS 38.473 section 8.4.1.
  /// \param[in] pdu contains the UL-CCCH message that is packed in the RRC container of the initial UL RRC message
  /// transfer message.
  void handle_pdu(byte_buffer_slice_chain pdu) override;

private:
  du_ue_index_t             ue_index;
  gnb_du_ue_f1ap_id_t       gnb_du_f1ap_ue_id;
  rnti_t                    c_rnti;
  const asn1::f1ap::nrcgi_s nr_cgi;
  byte_buffer               du_cu_rrc_container;
  f1c_message_notifier&     f1c_notifier;
  f1ap_event_manager&       ev_manager;
  srslog::basic_logger&     logger;
};

class f1c_other_srb_du_bearer final : public f1_du_bearer
{
public:
  f1c_other_srb_du_bearer(gnb_du_ue_f1ap_id_t   gnb_du_f1ap_ue_id_,
                          gnb_cu_ue_f1ap_id_t   gnb_cu_f1ap_ue_id_,
                          srb_id_t              srb_id_,
                          f1c_message_notifier& f1c_notifier_) :
    gnb_du_f1ap_ue_id(gnb_du_f1ap_ue_id_),
    gnb_cu_f1ap_ue_id(gnb_cu_f1ap_ue_id_),
    srb_id(srb_id_),
    f1c_notifier(f1c_notifier_)
  {
  }

  /// \brief Packs and forwards the UL RRC message transfer as per TS 38.473 section 8.4.3.
  /// \param[in] pdu The message to be encoded in the RRC container of the UL RRC message transfer message to transmit.
  void handle_pdu(byte_buffer_slice_chain pdu) override
  {
    f1c_message msg;

    // Fill F1AP UL RRC Message Transfer.
    msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_ULRRC_MSG_TRANSFER);
    asn1::f1ap::ulrrc_msg_transfer_s& ul_msg = msg.pdu.init_msg().value.ulrrc_msg_transfer();
    ul_msg->gnb_du_ue_f1_ap_id->value        = gnb_du_ue_f1ap_id_to_uint(gnb_du_f1ap_ue_id);
    ul_msg->gnb_cu_ue_f1_ap_id->value        = gnb_cu_ue_f1ap_id_to_uint(gnb_cu_f1ap_ue_id);
    ul_msg->srbid->value                     = srb_id_to_uint(srb_id);
    ul_msg->rrc_container->resize(pdu.length());
    std::copy(pdu.begin(), pdu.end(), ul_msg->rrc_container->begin());
    ul_msg->sel_plmnid_present              = false;
    ul_msg->new_g_nb_du_ue_f1_ap_id_present = false;

    f1c_notifier.on_new_message(msg);
  }

private:
  gnb_du_ue_f1ap_id_t   gnb_du_f1ap_ue_id;
  gnb_cu_ue_f1ap_id_t   gnb_cu_f1ap_ue_id;
  srb_id_t              srb_id;
  f1c_message_notifier& f1c_notifier;
};

} // namespace srs_du
} // namespace srsgnb