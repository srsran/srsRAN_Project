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

#include "../ue_context/f1ap_du_ue.h"
#include "../ue_context/f1ap_du_ue_manager.h"
#include "srsran/asn1/f1ap/f1ap.h"

namespace srsran {
namespace srs_du {

class f1ap_du_ue_context_release_procedure
{
public:
  f1ap_du_ue_context_release_procedure(const asn1::f1ap::ue_context_release_cmd_s& msg_, f1ap_du_ue_manager& ues);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  void send_ue_context_release_complete();

  const asn1::f1ap::ue_context_release_cmd_s msg;
  f1ap_du_ue_manager&                        ue_db;
  f1ap_du_ue&                                ue;
  srslog::basic_logger&                      logger = srslog::fetch_basic_logger("DU-F1");
};

} // namespace srs_du
} // namespace srsran
