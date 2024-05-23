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

#include "srsran/f1u/du/f1u_bearer.h"
#include "srsran/f1u/du/f1u_config.h"
#include "srsran/f1u/du/f1u_tx_pdu_notifier.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/timers.h"

namespace srsran::srs_du {

/// This class provides a notifier for the RX bearer
/// inside the DU F1-U gateway. This provides an adapter
/// to the NR-U bearer to pass SDUs into.
class f1u_du_gateway_bearer_rx_notifier
{
public:
  virtual ~f1u_du_gateway_bearer_rx_notifier() = default;

  virtual void on_new_pdu(nru_dl_message msg) = 0;
};

/// This class provides the interface for an F1-U GW bearer
class f1u_du_gateway_bearer : public srs_du::f1u_tx_pdu_notifier
{
  virtual void stop() = 0;
};

/// This class will be used to provide the interfaces to
/// the DU to create and manage F1-U bearers.
class f1u_du_gateway : public srs_du::f1u_bearer_disconnector
{
public:
  f1u_du_gateway()                                 = default;
  ~f1u_du_gateway() override                       = default;
  f1u_du_gateway(const f1u_du_gateway&)            = default;
  f1u_du_gateway& operator=(const f1u_du_gateway&) = default;
  f1u_du_gateway(f1u_du_gateway&&)                 = default;
  f1u_du_gateway& operator=(f1u_du_gateway&&)      = default;

  virtual std::unique_ptr<f1u_du_gateway_bearer> create_du_bearer(uint32_t                       ue_index,
                                                                  drb_id_t                       drb_id,
                                                                  srs_du::f1u_config             config,
                                                                  const up_transport_layer_info& dl_up_tnl_info,
                                                                  const up_transport_layer_info& ul_up_tnl_info,
                                                                  srs_du::f1u_du_gateway_bearer_rx_notifier& du_rx,
                                                                  timer_factory                              timers,
                                                                  task_executor& ue_executor) = 0;
};

} // namespace srsran::srs_du
