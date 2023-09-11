/*
 *
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
  auto bearer = std::make_unique<f1u_bearer_impl>(
      msg.ue_index, msg.drb_id, msg.dl_tnl_info, msg.config, *msg.rx_sdu_notifier, *msg.tx_pdu_notifier, msg.timers);
  return bearer;
}
