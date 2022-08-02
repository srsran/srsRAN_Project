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

#include "srsgnb/cu_cp/ue_context.h"
#include "srsgnb/rrc/rrc_factory.h"
#include "srsgnb/rrc/rrc_ue_entity_interface.h"

namespace srsgnb {

namespace srs_cu_cp {

/// Main UE representation in RRC
class rrc_ue_entity : public rrc_ue_entity_interface,
                      public rrc_ul_ccch_pdu_handler,
                      public rrc_ul_dcch_pdu_handler,
                      public rrc_ue_dl_nas_message_handler
{
public:
  rrc_ue_entity(rrc_entity_ue_interface&               parent_,
                rrc_ue_du_processor_notifier&          du_proc_notif_,
                const ue_index_t                       ue_index_,
                const rnti_t                           c_rnti_,
                const rrc_ue_cfg_t&                    cfg_,
                const srb_notifiers&                   srbs_,
                const asn1::unbounded_octstring<true>& du_to_cu_container,
                rrc_ue_task_scheduler&                 task_sched);
  ~rrc_ue_entity() = default;

  // rrc_ul_ccch_pdu_handler
  void handle_ul_ccch_pdu(byte_buffer_slice pdu) override;
  void handle_ul_dcch_pdu(byte_buffer_slice pdu) override;

  // rrc_ue_entity_interface
  rrc_ul_ccch_pdu_handler& get_ul_ccch_pdu_handler() override;
  rrc_ul_dcch_pdu_handler& get_ul_dcch_pdu_handler() override;
  void                     connect_srb_notifier(srb_id_t srb_id, rrc_pdu_notifier& notifier) override;

  // rrc_ue_dl_nas_message_handler
  void handle_dl_nas_transport_message(const dl_nas_transport_message& msg) override;

private:
  // message handlers
  void handle_rrc_setup_request(const asn1::rrc_nr::rrc_setup_request_s& msg);
  void handle_rrc_reest_request(const asn1::rrc_nr::rrc_reest_request_s& msg);

  // message senders
  /// \remark Send RRC Setup, see section 5.3.3 in TS 36.331
  void send_rrc_setup();

  /// \remark Send RRC Reject, see section 5.3.15 in TS 38.331
  void send_rrc_reject(uint8_t reject_wait_time_secs);

  // helpers
  /// Packs a DL-CCCH message and logs the message
  void send_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg);

  /// Packs a DL-DCCH message and logs the message
  void send_dl_dcch(const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg);

  /// Sends a packed PDU on an SRB
  void send_srb_pdu(srb_id_t srb_id, byte_buffer pdu);

  /// Sends a UE Context Release Request
  void send_ue_context_release_command();

  /// allocates PUCCH resources at the cell resource manager
  bool init_pucch();

  // Helper to create PDU from RRC message
  template <class T>
  byte_buffer pack_into_pdu(const T& msg, const char* context_name = nullptr)
  {
    context_name = context_name == nullptr ? __FUNCTION__ : context_name;
    byte_buffer   pdu{};
    asn1::bit_ref bref{pdu};
    if (msg.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
      cfg.logger.error("Failed to pack message in %s. Discarding it.", context_name);
    }
    return pdu;
  }

  // Logging
  typedef enum { Rx = 0, Tx } direction_t;
  template <class T>
  void
  log_rrc_message(const char* source, const direction_t dir, byte_buffer_view pdu, const T& msg, const char* msg_type);
  void
  log_rx_pdu_fail(uint16_t rnti, const char* source, byte_buffer_view pdu, const char* cause_str, bool log_hex = true);

  rrc_entity_ue_interface&      parent;                // reference to the parant RRC object
  rrc_ue_du_processor_notifier& du_processor_notifier; // notifier to the DU processor
  const ue_index_t              ue_index;              // UE index assigned by the DU processor
  const rnti_t                  c_rnti;                // current C-RNTI
  const rrc_ue_cfg_t&           cfg;
  srb_notifiers                 srbs;               // set notifiers for all SRBs
  byte_buffer                   du_to_cu_container; // initial RRC message from DU to CU
  rrc_ue_task_scheduler&        task_sched;

  struct rrc_ctxt_t {
    uint64_t                               setup_ue_id = -1;
    asn1::rrc_nr::establishment_cause_opts connection_cause;
  } rrc_ctxt;
};

} // namespace srs_cu_cp

} // namespace srsgnb
