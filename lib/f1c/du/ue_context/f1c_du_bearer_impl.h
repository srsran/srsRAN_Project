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

#include "f1ap_ue_context.h"
#include "srsgnb/f1c/common/f1c_types.h"
#include "srsgnb/f1c/du/f1ap_du.h"
#include "srsgnb/f1c/du/f1c_bearer.h"
#include "srsgnb/ran/rnti.h"

namespace srsgnb {
namespace srs_du {

class f1ap_event_manager;

class f1c_srb0_du_bearer final : public f1c_bearer
{
public:
  f1c_srb0_du_bearer(f1ap_ue_context&           ue_ctxt_,
                     const asn1::f1ap::nrcgi_s& nr_cgi_,
                     const byte_buffer&         du_cu_rrc_container_,
                     f1c_message_notifier&      f1c_notifier_,
                     f1c_tx_pdu_notifier&       f1c_pdu_notifier_,
                     f1ap_event_manager&        ev_manager_);

  /// \brief Packs and forwards the initial UL RRC message transfer as per TS 38.473 section 8.4.1.
  /// \param[in] pdu contains the UL-CCCH message that is packed in the RRC container of the initial UL RRC message
  /// transfer message.
  void handle_pdu(byte_buffer_slice_chain pdu) override;

  void handle_sdu(byte_buffer sdu) override;

private:
  f1ap_ue_context&          ue_ctxt;
  const asn1::f1ap::nrcgi_s nr_cgi;
  byte_buffer               du_cu_rrc_container;
  f1c_message_notifier&     f1c_notifier;
  f1c_tx_pdu_notifier&      pdu_notifier;
  f1ap_event_manager&       ev_manager;
  srslog::basic_logger&     logger;
};

class f1c_other_srb_du_bearer final : public f1c_bearer
{
public:
  f1c_other_srb_du_bearer(f1ap_ue_context&      ue_ctxt_,
                          srb_id_t              srb_id_,
                          f1c_message_notifier& f1c_notifier_,
                          f1c_tx_pdu_notifier&  f1c_pdu_notifier_);

  /// \brief Packs and forwards the UL RRC message transfer as per TS 38.473 section 8.4.3.
  /// \param[in] pdu The message to be encoded in the RRC container of the UL RRC message transfer message to transmit.
  void handle_pdu(byte_buffer_slice_chain pdu) override;

  void handle_sdu(byte_buffer sdu) override;

private:
  f1ap_ue_context&      ue_ctxt;
  srb_id_t              srb_id;
  f1c_message_notifier& f1c_notifier;
  f1c_tx_pdu_notifier&  pdu_notifier;
  srslog::basic_logger& logger;
};

} // namespace srs_du
} // namespace srsgnb