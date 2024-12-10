/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cu_cp_impl_interface.h"
#include "../ue_manager/ue_manager_impl.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Removes a UE from the CU-CP
class ue_removal_routine
{
public:
  ue_removal_routine(ue_index_t                           ue_index_,
                     rrc_ue_handler&                      rrc_du_notifier_,
                     e1ap_bearer_context_removal_handler* e1ap_removal_handler_,
                     f1ap_ue_context_removal_handler&     f1ap_removal_handler_,
                     ngap_ue_context_removal_handler&     ngap_removal_handler_,
                     nrppa_ue_context_removal_handler*    nrppa_removal_handler_,
                     ue_manager&                          ue_mng_,
                     srslog::basic_logger&                logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "UE Removal Routine"; }

private:
  const ue_index_t ue_index;
  // Handlers to trigger UE removal in the respective layers.
  rrc_ue_handler&                      rrc_du_notifier;
  e1ap_bearer_context_removal_handler* e1ap_removal_handler = nullptr;
  f1ap_ue_context_removal_handler&     f1ap_removal_handler;
  ngap_ue_context_removal_handler&     ngap_removal_handler;
  nrppa_ue_context_removal_handler*    nrppa_removal_handler = nullptr;
  // To remove UE context from DU processor.
  ue_manager&           ue_mng;
  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsran
