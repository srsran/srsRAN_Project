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

#include "srsran/f1u/cu_up/f1u_bearer.h"
#include "srsran/f1u/cu_up/f1u_config.h"
#include "srsran/f1u/cu_up/f1u_rx_delivery_notifier.h"
#include "srsran/f1u/cu_up/f1u_rx_sdu_notifier.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/timers.h"

namespace srsran {

/// This class provides an interface for the TX bearer
/// inside the CU-UP F1-U gateway. In the case of a co-located
/// deployment this will be an adapter that directly connects
/// to the DU F1-U gateway bearer, in the case of a split deployment,
/// this will be an NR-U GTP-U tunnel.
class f1u_cu_up_gateway_bearer_tx_interface
{
public:
  virtual ~f1u_cu_up_gateway_bearer_tx_interface() = default;

  virtual void on_new_sdu(nru_dl_message msg) = 0;
};

/// This class provides a notifier for the RX bearer
/// inside the CU-UP F1-U gateway. This provides an adapter
/// to the NR-U bearer to pass SDUs into.
class f1u_cu_up_gateway_bearer_rx_notifier
{
public:
  virtual ~f1u_cu_up_gateway_bearer_rx_notifier() = default;

  virtual void on_new_pdu(nru_ul_message msg) = 0;
};

/// This class will be used to provide the interfaces to
/// the CU-UP to create and manage F1-U bearers.
class f1u_cu_up_gateway : public srs_cu_up::f1u_bearer_disconnector
{
public:
  f1u_cu_up_gateway()                                    = default;
  virtual ~f1u_cu_up_gateway()                           = default;
  f1u_cu_up_gateway(const f1u_cu_up_gateway&)            = default;
  f1u_cu_up_gateway& operator=(const f1u_cu_up_gateway&) = default;
  f1u_cu_up_gateway(f1u_cu_up_gateway&&)                 = default;
  f1u_cu_up_gateway& operator=(f1u_cu_up_gateway&&)      = default;

  virtual f1u_cu_up_gateway_bearer_tx_interface* create_cu_bearer(uint32_t                              ue_index,
                                                                  drb_id_t                              drb_id,
                                                                  const srs_cu_up::f1u_config&          config,
                                                                  const up_transport_layer_info&        ul_up_tnl_info,
                                                                  f1u_cu_up_gateway_bearer_rx_notifier& rx_notifier,
                                                                  task_executor&                        ul_exec,
                                                                  timer_factory ue_dl_timer_factory,
                                                                  unique_timer& ue_inactivity_timer) = 0;

  virtual void attach_dl_teid(const up_transport_layer_info& ul_up_tnl_info,
                              const up_transport_layer_info& dl_up_tnl_info) = 0;
};

} // namespace srsran
