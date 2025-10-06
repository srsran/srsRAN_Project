/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cu_up_processor/cu_up_processor_repository.h"
#include "../ue_manager/ue_manager_impl.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/plmn_identity.h"

namespace srsran {
namespace srs_cu_cp {

class ue_transaction_info_release_routine
{
public:
  ue_transaction_info_release_routine(const ue_transaction_info_loss_event& loss_event_,
                                      ue_manager&                           ue_mng_,
                                      ngap_repository&                      ngap_db_,
                                      cu_up_processor_repository&           cu_up_db_,
                                      cu_cp_ue_removal_handler&             ue_rem_handler_,
                                      srslog::basic_logger&                 logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "UE Transaction Info Release Routine"; }

private:
  void prepare_ng_reset_messages();
  void prepare_e1_reset_messages();
  void launch_ue_removal(ue_index_t ue_idx);

  const ue_transaction_info_loss_event loss_event;
  ue_manager&                          ue_mng;
  ngap_repository&                     ngap_db;
  cu_up_processor_repository&          cu_up_db;
  cu_cp_ue_removal_handler&            ue_rem_handler;
  srslog::basic_logger&                logger;

  // NG reset messages per PLMN.
  std::map<plmn_identity, cu_cp_reset>           ng_reset_per_plmn;
  std::map<plmn_identity, cu_cp_reset>::iterator plmn_id_it;
  ngap_interface*                                ngap = nullptr;

  // E1 reset messages per CU-UP.
  std::map<cu_up_index_t, cu_cp_reset>           e1_reset_per_cu_up;
  std::map<cu_up_index_t, cu_cp_reset>::iterator cu_up_id_it;
  cu_up_processor*                               cu_up = nullptr;

  unsigned          ues_remaining_count{0};
  manual_event_flag all_ues_reset;
};

} // namespace srs_cu_cp
} // namespace srsran
