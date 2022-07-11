/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_RRC_UE_RRC_UE_ENTITY_H
#define SRSGNB_LIB_RRC_UE_RRC_UE_ENTITY_H

#include "srsgnb/cu_cp/ue_context.h"
#include "srsgnb/rrc/rrc_factory.h"
#include "srsgnb/rrc/rrc_ue_entity_interface.h"

namespace srsgnb {

namespace srs_cu_cp {

/// Main UE representation in RRC
class rrc_ue_entity : public rrc_ue_entity_interface, public rrc_ul_ccch_pdu_handler
{
public:
  rrc_ue_entity(rrc_entity_ue_interface& parent_, ue_context& ctxt_, const rrc_ue_cfg_t& cfg_);
  ~rrc_ue_entity() = default;

  // rrc_ul_ccch_pdu_handler
  void handle_ul_ccch_pdu(byte_buffer_slice pdu) override;

  // rrc_ue_entity_interface
  rrc_ul_ccch_pdu_handler* get_ul_ccch_pdu_handler() override;

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
  /// Packs a DL-DCCH message and logs the message
  void send_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg);

  /// Sends a packed PDU on SRB0
  void send_srb0_pdu(byte_buffer pdu);

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
  void log_rx_pdu_fail(uint16_t rnti, uint32_t lcid, byte_buffer_view pdu, const char* cause_str, bool log_hex = true);

  rrc_entity_ue_interface& parent; // reference to the parant RRC object
  ue_context&              ctxt;   // reference to the UE object
  const rrc_ue_cfg_t&      cfg;

  struct rrc_ctxt_t {
    uint64_t                               setup_ue_id = -1;
    asn1::rrc_nr::establishment_cause_opts connection_cause;
  } rrc_ctxt;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_LIB_RRC_UE_RRC_UE_ENTITY_H
