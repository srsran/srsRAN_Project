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

#include "../ngap_context.h"
#include "ngap_transaction_manager.h"
#include "srsran/cu_cp/ue_manager.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_handover_preparation_procedure
{
public:
  ngap_handover_preparation_procedure(const ngap_handover_preparation_request& req_,
                                      ngap_context_t&                          context_,
                                      ngap_ue_manager&                         ue_manager_,
                                      ngap_message_notifier&                   amf_notif_,
                                      ngap_transaction_manager&                ev_mng_,
                                      timer_factory                            timers,
                                      srslog::basic_logger&                    logger_);

  void operator()(coro_context<async_task<ngap_handover_preparation_response>>& ctx);

private:
  const ngap_handover_preparation_request request;
  ngap_context_t&                         context;
  ngap_ue_manager&                        ue_manager;
  ngap_message_notifier&                  amf_notifier;
  ngap_transaction_manager&               ev_mng;
  srslog::basic_logger&                   logger;

  unique_timer tng_reloc_prep_timer;

  ngap_ue*                                                                                ue;
  protocol_transaction_outcome_observer<asn1::ngap::ho_cmd_s, asn1::ngap::ho_prep_fail_s> transaction_sink;

  void send_handover_required();

  // ASN.1 helpers
  void fill_asn1_target_ran_node_id(asn1::ngap::target_id_c& target_id);
  void fill_asn1_pdu_session_res_list(asn1::ngap::pdu_session_res_list_ho_rqd_l& pdu_session_res_list);
};

} // namespace srs_cu_cp
} // namespace srsran
