/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "f1ap_ue_context.h"
#include "srsran/f1ap/common/f1ap_types.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/f1ap/du/f1c_bearer.h"
#include "srsran/ran/rnti.h"

namespace srsran {
namespace srs_du {

class f1ap_event_manager;

class f1c_srb0_du_bearer final : public f1c_bearer
{
public:
  f1c_srb0_du_bearer(f1ap_ue_context&            ue_ctxt_,
                     const asn1::f1ap::nr_cgi_s& nr_cgi_,
                     const byte_buffer&          du_cu_rrc_container_,
                     f1ap_message_notifier&      f1ap_notifier_,
                     f1c_rx_sdu_notifier&        f1c_rx_sdu_notifier_,
                     f1ap_event_manager&         ev_manager_);

  /// \brief Packs and forwards the initial UL RRC message transfer as per TS 38.473 section 8.4.1.
  /// \param[in] sdu contains the UL-CCCH message that is packed in the RRC container of the initial UL RRC message
  /// transfer message.
  void handle_sdu(byte_buffer_slice_chain sdu) override;

  void handle_pdu(byte_buffer pdu) override;

private:
  f1ap_ue_context&           ue_ctxt;
  const asn1::f1ap::nr_cgi_s nr_cgi;
  byte_buffer                du_cu_rrc_container;
  f1ap_message_notifier&     f1ap_notifier;
  f1c_rx_sdu_notifier&       sdu_notifier;
  f1ap_event_manager&        ev_manager;
  srslog::basic_logger&      logger;
};

class f1c_other_srb_du_bearer final : public f1c_bearer
{
public:
  f1c_other_srb_du_bearer(f1ap_ue_context&       ue_ctxt_,
                          srb_id_t               srb_id_,
                          f1ap_message_notifier& f1ap_notifier_,
                          f1c_rx_sdu_notifier&   f1c_sdu_notifier_);

  /// \brief Packs and forwards the UL RRC message transfer as per TS 38.473 section 8.4.3.
  /// \param[in] sdu The message to be encoded in the RRC container of the UL RRC message transfer message to transmit.
  void handle_sdu(byte_buffer_slice_chain sdu) override;

  void handle_pdu(byte_buffer sdu) override;

private:
  f1ap_ue_context&       ue_ctxt;
  srb_id_t               srb_id;
  f1ap_message_notifier& f1ap_notifier;
  f1c_rx_sdu_notifier&   sdu_notifier;
  srslog::basic_logger&  logger;
};

} // namespace srs_du
} // namespace srsran