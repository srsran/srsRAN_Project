/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/f1u/du/f1u_bearer.h"
#include "srsran/f1u/du/f1u_config.h"
#include "srsran/f1u/du/f1u_rx_sdu_notifier.h"
#include "srsran/gtpu/gtpu_teid.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace srs_du {

/// This class will be used to provide the interfaces to
/// the DU to create and manage F1-U bearers.
class f1u_du_gateway
{
public:
  f1u_du_gateway()                                 = default;
  virtual ~f1u_du_gateway()                        = default;
  f1u_du_gateway(const f1u_du_gateway&)            = default;
  f1u_du_gateway& operator=(const f1u_du_gateway&) = default;
  f1u_du_gateway(f1u_du_gateway&&)                 = default;
  f1u_du_gateway& operator=(f1u_du_gateway&&)      = default;

  virtual srs_du::f1u_bearer* create_du_bearer(uint32_t                       ue_index,
                                               drb_id_t                       drb_id,
                                               srs_du::f1u_config             config,
                                               const up_transport_layer_info& dl_up_tnl_info,
                                               const up_transport_layer_info& ul_up_tnl_info,
                                               srs_du::f1u_rx_sdu_notifier&   du_rx,
                                               timer_factory                  timers) = 0;

  virtual void remove_du_bearer(const up_transport_layer_info& dl_up_tnl_info) = 0;
};

} // namespace srs_du
} // namespace srsran
