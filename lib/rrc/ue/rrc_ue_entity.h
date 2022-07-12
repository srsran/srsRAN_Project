/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RRC_UE_ENTITY_H
#define SRSGNB_RRC_UE_ENTITY_H

#include "srsgnb/cu_cp/ue_context.h"
#include "srsgnb/rrc/rrc_factory.h"
#include "srsgnb/rrc/rrc_ue_entity_interface.h"

namespace srsgnb {

namespace srs_cu_cp {

/// Main UE representation in RRC
class rrc_ue_entity : public rrc_ue_entity_interface, public rrc_ul_ccch_pdu_handler
{
public:
  rrc_ue_entity(ue_context& ctxt_, const rrc_ue_cfg_t& cfg_);
  ~rrc_ue_entity() = default;

  // rrc_ul_ccch_pdu_handler
  void handle_ul_ccch_pdu(byte_buffer_slice pdu) override;

  // rrc_ue_entity_interface
  rrc_ul_ccch_pdu_handler* get_ul_ccch_pdu_handler() override;

private:
  // message handlers
  void handle_rrc_setup_request(const asn1::rrc_nr::rrc_setup_request_s& msg);
  void handle_rrc_reest_request(const asn1::rrc_nr::rrc_reest_request_s& msg);

  // senders
  void send_srb0_pdu(byte_buffer pdu);
  void send_rrc_setup();

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
       log_rrc_message(const char* source, const direction_t dir, byte_buffer_slice pdu, const T& msg, const char* msg_type);
  void log_rx_pdu_fail(uint16_t rnti, uint32_t lcid, byte_buffer_slice pdu, const char* cause_str, bool log_hex = true);

  ue_context&         ctxt; // reference to the UE object
  const rrc_ue_cfg_t& cfg;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_RRC_UE_ENTITY_H
