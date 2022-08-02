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

rrc_ue_entity::rrc_ue_entity(rrc_entity_ue_interface&               parent_,
                             rrc_ue_du_processor_notifier&          du_proc_notif_,
                             rrc_ue_ngap_notifier&                  ngap_notif_,
                             const ue_index_t                       ue_index_,
                             const rnti_t                           c_rnti_,
                             const rrc_ue_cfg_t&                    cfg_,
                             const srb_notifiers&                   srbs_,
                             const asn1::unbounded_octstring<true>& du_to_cu_container_,
                             rrc_ue_task_scheduler&                 task_sched_) :
  parent(parent_),
  du_processor_notifier(du_proc_notif_),
  ngap_notifier(ngap_notif_),
  ue_index(ue_index_),
  c_rnti(c_rnti_),
  cfg(cfg_),
  srbs(srbs_),
  du_to_cu_container(du_to_cu_container_),
  task_sched(task_sched_)
{
  // TODO: Use task_sched to schedule RRC procedures.
  (void)task_sched;
}

rrc_ul_ccch_pdu_handler& rrc_ue_entity::get_ul_ccch_pdu_handler()
{
  return *this;
}

rrc_ul_dcch_pdu_handler& rrc_ue_entity::get_ul_dcch_pdu_handler()
{
  return *this;
}

void rrc_ue_entity::connect_srb_notifier(srb_id_t srb_id, rrc_pdu_notifier& notifier)
{
  if (srb_id >= MAX_NOF_SRBS) {
    cfg.logger.error("Couldn't connect notifier for SRB{}", srb_id);
  }
  srbs[srb_id] = &notifier;
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
template void rrc_ue_entity::log_rrc_message<ul_dcch_msg_s>(const char*          source,
                                                            const direction_t    dir,
                                                            byte_buffer_view     pdu,
                                                            const ul_dcch_msg_s& msg,
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
                                    const char*      source,
                                    byte_buffer_view pdu,
                                    const char*      cause_str,
                                    bool             log_hex)
{
  if (log_hex) {
    std::vector<uint8_t> bytes{pdu.begin(), pdu.end()};
    cfg.logger.error(
        bytes.data(), bytes.size(), "Rx {} PDU, rnti=0x{:x}} - Discarding. Cause: {}", source, rnti, cause_str);
  } else {
    cfg.logger.error("Rx {} PDU, rnti=0x{:x} - Discarding. Cause: {}", source, rnti, cause_str);
  }
}