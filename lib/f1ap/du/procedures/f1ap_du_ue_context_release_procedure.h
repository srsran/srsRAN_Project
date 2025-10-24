/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "../ue_context/f1ap_du_ue.h"
#include "../ue_context/f1ap_du_ue_manager.h"
#include "srsran/asn1/f1ap/f1ap.h"

namespace srsran {
namespace srs_du {

struct f1ap_du_context;

class f1ap_du_ue_context_release_procedure
{
public:
  f1ap_du_ue_context_release_procedure(const asn1::f1ap::ue_context_release_cmd_s& msg_,
                                       f1ap_du_ue_manager&                         ues,
                                       const f1ap_du_context&                      ctxt_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char* name() const { return "UE Context Release"; }

  void send_ue_context_release_complete();

  async_task<bool> handle_rrc_container();

  const asn1::f1ap::ue_context_release_cmd_s msg;
  f1ap_du_ue&                                ue;
  srslog::basic_logger&                      logger = srslog::fetch_basic_logger("DU-F1");
  f1ap_message_notifier&                     cu_msg_notifier; // used after the UE context as been released.
  const f1ap_du_context&                     du_ctxt;
};

} // namespace srs_du
} // namespace srsran
