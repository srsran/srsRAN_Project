/*
 *
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/f1u/cu_up/f1u_bearer_factory.h"
#include "f1u_bearer_impl.h"

using namespace srsran;
using namespace srs_cu_up;

std::unique_ptr<f1u_bearer> srsran::srs_cu_up::create_f1u_bearer(uint32_t                       ue_index,
                                                                 drb_id_t                       drb_id,
                                                                 const up_transport_layer_info& ul_up_tnl_info,
                                                                 const f1u_config&              config,
                                                                 f1u_tx_pdu_notifier&           tx_pdu_notifier,
                                                                 f1u_rx_delivery_notifier&      rx_delivery_notifier,
                                                                 f1u_rx_sdu_notifier&           rx_sdu_notifier,
                                                                 timer_factory                  ue_dl_timer_factory,
                                                                 unique_timer&                  ue_inactivity_timer,
                                                                 task_executor&                 ul_exec)
{
  auto bearer = std::make_unique<f1u_bearer_impl>(ue_index,
                                                  drb_id,
                                                  ul_up_tnl_info,
                                                  config,
                                                  tx_pdu_notifier,
                                                  rx_delivery_notifier,
                                                  rx_sdu_notifier,
                                                  ue_dl_timer_factory,
                                                  ue_inactivity_timer,
                                                  ul_exec);
  return bearer;
}
