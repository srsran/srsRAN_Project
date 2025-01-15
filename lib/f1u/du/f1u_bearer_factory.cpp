/*
 *
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/f1u/du/f1u_bearer_factory.h"
#include "f1u_bearer_impl.h"

using namespace srsran;
using namespace srs_du;

std::unique_ptr<f1u_bearer> srsran::srs_du::create_f1u_bearer(const f1u_bearer_creation_message& msg)
{
  srsran_assert(msg.rx_sdu_notifier != nullptr, "Cannot create F1-U bearer: RX SDU notifier is not configured.");
  srsran_assert(msg.tx_pdu_notifier != nullptr, "Cannot create F1-U bearer: TX PDU notifier is not configured.");
  srsran_assert(msg.ue_executor != nullptr, "Cannot create F1-U bearer: UE executor is not configured.");
  srsran_assert(msg.disconnector != nullptr, "Cannot create F1-U bearer: disconnector is not configured.");
  auto bearer = std::make_unique<f1u_bearer_impl>(msg.ue_index,
                                                  msg.drb_id,
                                                  msg.dl_tnl_info,
                                                  msg.config,
                                                  *msg.rx_sdu_notifier,
                                                  *msg.tx_pdu_notifier,
                                                  msg.timers,
                                                  *msg.ue_executor);
  return bearer;
}
