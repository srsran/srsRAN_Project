/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_ue_impl.h"
#include "../../ran/gnb_format.h"

using namespace srsgnb;
using namespace srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_ue_impl::rrc_ue_impl(rrc_du_ue_manager&                     rrc_du_,
                         rrc_ue_du_processor_notifier&          du_proc_notif_,
                         rrc_ue_nas_notifier&                   nas_notif_,
                         const ue_index_t                       ue_index_,
                         const rnti_t                           c_rnti_,
                         const rrc_ue_cfg_t&                    cfg_,
                         const srb_notifiers&                   srbs_,
                         const asn1::unbounded_octstring<true>& du_to_cu_container_,
                         rrc_ue_task_scheduler&                 task_sched_) :
  context(ue_index_, c_rnti_, cfg_),
  rrc_du(rrc_du_),
  du_processor_notifier(du_proc_notif_),
  nas_notifier(nas_notif_),
  srbs(srbs_),
  du_to_cu_container(du_to_cu_container_),
  task_sched(task_sched_),
  logger(cfg_.logger),
  event_mng(std::make_unique<rrc_ue_event_manager>(task_sched_.get_timer_manager()))
{
  // TODO: Use task_sched to schedule RRC procedures.
  (void)task_sched;
}

rrc_ul_ccch_pdu_handler& rrc_ue_impl::get_ul_ccch_pdu_handler()
{
  return *this;
}

rrc_ul_dcch_pdu_handler& rrc_ue_impl::get_ul_dcch_pdu_handler()
{
  return *this;
}

void rrc_ue_impl::connect_srb_notifier(srb_id_t srb_id, rrc_pdu_notifier& notifier)
{
  if (srb_id_to_uint(srb_id) >= MAX_NOF_SRBS) {
    logger.error("Couldn't connect notifier for SRB{}", srb_id);
  }
  srbs[srb_id_to_uint(srb_id)] = &notifier;
}

void rrc_ue_impl::on_new_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg)
{
  send_dl_ccch(dl_ccch_msg);
}

template <class T>
void rrc_ue_impl::log_rrc_message(const char*       source,
                                  const direction_t dir,
                                  byte_buffer_view  pdu,
                                  const T&          msg,
                                  const char*       msg_type)
{
  if (logger.debug.enabled()) {
    asn1::json_writer json_writer;
    msg.to_json(json_writer);
    // TODO: remove serialization
    std::vector<uint8_t> bytes{pdu.begin(), pdu.end()};
    logger.debug(
        bytes.data(), bytes.size(), "{} - {} {} ({} B)", source, (dir == Rx) ? "Rx" : "Tx", msg_type, pdu.length());
    logger.debug("Content: {}", json_writer.to_string().c_str());
  } else if (logger.info.enabled()) {
    std::vector<uint8_t> bytes{pdu.begin(), pdu.end()};
    logger.info(
        bytes.data(), bytes.size(), "{} - {} {} ({} B)", source, (dir == Rx) ? "Rx" : "Tx", msg_type, pdu.length());
  }
}
template void rrc_ue_impl::log_rrc_message<ul_ccch_msg_s>(const char*          source,
                                                          const direction_t    dir,
                                                          byte_buffer_view     pdu,
                                                          const ul_ccch_msg_s& msg,
                                                          const char*          msg_type);
template void rrc_ue_impl::log_rrc_message<ul_dcch_msg_s>(const char*          source,
                                                          const direction_t    dir,
                                                          byte_buffer_view     pdu,
                                                          const ul_dcch_msg_s& msg,
                                                          const char*          msg_type);
template void rrc_ue_impl::log_rrc_message<dl_ccch_msg_s>(const char*          source,
                                                          const direction_t    dir,
                                                          byte_buffer_view     pdu,
                                                          const dl_ccch_msg_s& msg,
                                                          const char*          msg_type);
template void rrc_ue_impl::log_rrc_message<dl_dcch_msg_s>(const char*          source,
                                                          const direction_t    dir,
                                                          byte_buffer_view     pdu,
                                                          const dl_dcch_msg_s& msg,
                                                          const char*          msg_type);
template void rrc_ue_impl::log_rrc_message<cell_group_cfg_s>(const char*             source,
                                                             const direction_t       dir,
                                                             byte_buffer_view        pdu,
                                                             const cell_group_cfg_s& msg,
                                                             const char*             msg_type);
template void rrc_ue_impl::log_rrc_message<radio_bearer_cfg_s>(const char*               source,
                                                               const direction_t         dir,
                                                               byte_buffer_view          pdu,
                                                               const radio_bearer_cfg_s& msg,
                                                               const char*               msg_type);

void rrc_ue_impl::log_rx_pdu_fail(uint16_t         rnti,
                                  const char*      source,
                                  byte_buffer_view pdu,
                                  const char*      cause_str,
                                  bool             log_hex)
{
  if (log_hex) {
    std::vector<uint8_t> bytes{pdu.begin(), pdu.end()};
    logger.error(
        bytes.data(), bytes.size(), "Rx {} PDU, rnti=0x{:x}} - Discarding. Cause: {}", source, rnti, cause_str);
  } else {
    logger.error("Rx {} PDU, rnti=0x{:x} - Discarding. Cause: {}", source, rnti, cause_str);
  }
}