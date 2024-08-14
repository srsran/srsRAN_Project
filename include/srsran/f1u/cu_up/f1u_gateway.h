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
#include "srsran/f1u/cu_up/f1u_tx_pdu_notifier.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// This class provides a notifier for the RX bearer
/// inside the CU-UP F1-U gateway. This provides an adapter
/// to the NR-U bearer to pass SDUs into.
class f1u_cu_up_gateway_bearer_rx_notifier
{
public:
  virtual ~f1u_cu_up_gateway_bearer_rx_notifier() = default;

  virtual void on_new_pdu(nru_ul_message msg) = 0;
};

/// This class provides the interface for an F1-U GW bearer
class f1u_cu_up_gateway_bearer : public srs_cu_up::f1u_tx_pdu_notifier
{
public:
  virtual void stop() = 0;
};

/// This class will be used to provide the interfaces to
/// the CU-UP to create and manage F1-U bearers.
class f1u_cu_up_gateway : public srs_cu_up::f1u_bearer_disconnector
{
public:
  f1u_cu_up_gateway()                                    = default;
  ~f1u_cu_up_gateway() override                          = default;
  f1u_cu_up_gateway(const f1u_cu_up_gateway&)            = default;
  f1u_cu_up_gateway& operator=(const f1u_cu_up_gateway&) = default;
  f1u_cu_up_gateway(f1u_cu_up_gateway&&)                 = default;
  f1u_cu_up_gateway& operator=(f1u_cu_up_gateway&&)      = default;

  virtual std::unique_ptr<f1u_cu_up_gateway_bearer> create_cu_bearer(uint32_t                       ue_index,
                                                                     drb_id_t                       drb_id,
                                                                     const srs_cu_up::f1u_config&   config,
                                                                     const up_transport_layer_info& ul_up_tnl_info,
                                                                     f1u_cu_up_gateway_bearer_rx_notifier& rx_notifier,
                                                                     task_executor&                        ul_exec) = 0;

  virtual void attach_dl_teid(const up_transport_layer_info& ul_up_tnl_info,
                              const up_transport_layer_info& dl_up_tnl_info) = 0;
};

/// This class will be used to provide the interfaces to
/// the CU-UP to create and manage F1-U bearers.
class f1u_cu_up_udp_gateway : public f1u_cu_up_gateway
{
public:
  f1u_cu_up_udp_gateway()                                        = default;
  ~f1u_cu_up_udp_gateway() override                              = default;
  f1u_cu_up_udp_gateway(const f1u_cu_up_udp_gateway&)            = default;
  f1u_cu_up_udp_gateway& operator=(const f1u_cu_up_udp_gateway&) = default;
  f1u_cu_up_udp_gateway(f1u_cu_up_udp_gateway&&)                 = default;
  f1u_cu_up_udp_gateway& operator=(f1u_cu_up_udp_gateway&&)      = default;

  virtual std::optional<uint16_t> get_bind_port() const = 0;
};

} // namespace srsran
