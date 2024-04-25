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
#include "srsran/f1u/du/f1u_rx_sdu_notifier.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace srs_du {

/// This class will be used to provide the interfaces to
/// the DU to create and manage F1-U bearers.
class f1u_du_gateway : public srs_du::f1u_bearer_disconnector
{
public:
  f1u_du_gateway()                                 = default;
  virtual ~f1u_du_gateway()                        = default;
  f1u_du_gateway(const f1u_du_gateway&)            = default;
  f1u_du_gateway& operator=(const f1u_du_gateway&) = default;
  f1u_du_gateway(f1u_du_gateway&&)                 = default;
  f1u_du_gateway& operator=(f1u_du_gateway&&)      = default;

  virtual std::unique_ptr<srs_du::f1u_bearer> create_du_bearer(uint32_t                       ue_index,
                                                               drb_id_t                       drb_id,
                                                               srs_du::f1u_config             config,
                                                               const up_transport_layer_info& dl_up_tnl_info,
                                                               const up_transport_layer_info& ul_up_tnl_info,
                                                               srs_du::f1u_rx_sdu_notifier&   du_rx,
                                                               timer_factory                  timers,
                                                               task_executor&                 ue_executor) = 0;
};

} // namespace srs_du
} // namespace srsran
