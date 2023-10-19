/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../ue_context/ngap_ue_context.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_dl_nas_message_transfer_procedure
{
public:
  ngap_dl_nas_message_transfer_procedure(byte_buffer               nas_pdu_,
                                         const ngap_ue_ids&        ue_ids_,
                                         ngap_rrc_ue_pdu_notifier& rrc_ue_pdu_notifier_,
                                         srslog::basic_logger&     logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "DL NAS Message Transfer Procedure"; }

private:
  // results senders
  void send_pdu_to_rrc_ue();

  byte_buffer               nas_pdu;
  const ngap_ue_ids         ue_ids;
  ngap_rrc_ue_pdu_notifier& rrc_ue_pdu_notifier;
  srslog::basic_logger&     logger;
};

} // namespace srs_cu_cp
} // namespace srsran
