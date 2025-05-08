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

#include "../ngap_error_indication_helper.h"
#include "../ue_context/ngap_ue_context.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_ue_context_release_procedure
{
public:
  ngap_ue_context_release_procedure(
      const cu_cp_ue_context_release_command&                     command_,
      const ngap_ue_ids&                                          ue_ids_,
      std::unordered_map<ue_index_t, error_indication_request_t>& stored_error_indications_,
      ngap_cu_cp_notifier&                                        cu_cp_notifier_,
      ngap_message_notifier&                                      amf_notifier_,
      ngap_ue_logger&                                             logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "UE Context Release Procedure"; }

private:
  // Result senders.
  bool send_ue_context_release_complete();

  cu_cp_ue_context_release_command                            command;
  const ngap_ue_ids                                           ue_ids;
  std::unordered_map<ue_index_t, error_indication_request_t>& stored_error_indications;
  cu_cp_ue_context_release_complete                           ue_context_release_complete;
  ngap_cu_cp_notifier&                                        cu_cp_notifier;
  ngap_message_notifier&                                      amf_notifier;
  ngap_ue_logger                                              logger;
};

} // namespace srs_cu_cp
} // namespace srsran
