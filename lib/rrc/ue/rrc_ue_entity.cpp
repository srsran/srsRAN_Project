/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_ue_entity.h"
#include "../../ran/gnb_format.h"

using namespace srsgnb;
using namespace srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_ue_entity::rrc_ue_entity(ue_context& ctxt_, const rrc_ue_cfg_t& cfg_) : ctxt(ctxt_), cfg(cfg_) {}

rrc_ul_ccch_pdu_handler* rrc_ue_entity::get_ul_ccch_pdu_handler()
{
  return this;
}

template <class T>
void rrc_ue_entity::log_rrc_message(const char*       source,
                                    const direction_t dir,
                                    byte_buffer_view  pdu,
                                    const T&          msg,
                                    const char*       msg_type)
{
  if (cfg.logger.debug.enabled()) {
    asn1::json_writer json_writer;
    msg.to_json(json_writer);
    // TODO: remove serialization
    std::vector<uint8_t> bytes{pdu.begin(), pdu.end()};
    cfg.logger.debug(
        bytes.data(), bytes.size(), "{} - {} {} ({} B)", source, (dir == Rx) ? "Rx" : "Tx", msg_type, pdu.length());
    cfg.logger.debug("Content: {}", json_writer.to_string().c_str());
  } else if (cfg.logger.info.enabled()) {
    std::vector<uint8_t> bytes{pdu.begin(), pdu.end()};
    cfg.logger.info(
        bytes.data(), bytes.size(), "{} - {} {} ({} B)", source, (dir == Rx) ? "Rx" : "Tx", msg_type, pdu.length());
  }
}
template void rrc_ue_entity::log_rrc_message<ul_ccch_msg_s>(const char*          source,
                                                            const direction_t    dir,
                                                            byte_buffer_view     pdu,
                                                            const ul_ccch_msg_s& msg,
                                                            const char*          msg_type);
template void rrc_ue_entity::log_rrc_message<dl_ccch_msg_s>(const char*          source,
                                                            const direction_t    dir,
                                                            byte_buffer_view     pdu,
                                                            const dl_ccch_msg_s& msg,
                                                            const char*          msg_type);
template void rrc_ue_entity::log_rrc_message<dl_dcch_msg_s>(const char*          source,
                                                            const direction_t    dir,
                                                            byte_buffer_view     pdu,
                                                            const dl_dcch_msg_s& msg,
                                                            const char*          msg_type);
template void rrc_ue_entity::log_rrc_message<cell_group_cfg_s>(const char*             source,
                                                               const direction_t       dir,
                                                               byte_buffer_view        pdu,
                                                               const cell_group_cfg_s& msg,
                                                               const char*             msg_type);
template void rrc_ue_entity::log_rrc_message<radio_bearer_cfg_s>(const char*               source,
                                                                 const direction_t         dir,
                                                                 byte_buffer_view          pdu,
                                                                 const radio_bearer_cfg_s& msg,
                                                                 const char*               msg_type);

void rrc_ue_entity::log_rx_pdu_fail(uint16_t         rnti,
                                    uint32_t         lcid,
                                    byte_buffer_view pdu,
                                    const char*      cause_str,
                                    bool             log_hex)
{
  if (log_hex) {
    std::vector<uint8_t> bytes{pdu.begin(), pdu.end()};
    cfg.logger.error(
        bytes.data(), bytes.size(), "Rx {} PDU, rnti=0x{:x}} - Discarding. Cause: {}", lcid, rnti, cause_str);
  } else {
    cfg.logger.error("Rx {} PDU, rnti=0x{:x} - Discarding. Cause: {}", lcid, rnti, cause_str);
  }
}