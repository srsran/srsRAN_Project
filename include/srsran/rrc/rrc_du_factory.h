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

#include "rrc_config.h"
#include "rrc_ue_config.h"
#include "srsran/ngap/ngap.h"
#include "srsran/rrc/rrc.h"
#include "srsran/rrc/rrc_du.h"
#include "srsran/rrc/rrc_ue.h"
#include <map>
#include <memory>

namespace srsran {

namespace srs_cu_cp {

struct ue_context;

struct rrc_du_creation_message {
  rrc_du_creation_message(const rrc_cfg_t&                    cfg_,
                          rrc_ue_du_processor_notifier&       rrc_ue_du_proc_notif_,
                          rrc_ue_nas_notifier&                nas_notif_,
                          rrc_ue_control_notifier&            ngap_ctrl_notif_,
                          rrc_du_measurement_config_notifier& rrc_du_cu_cp_notifier_) :
    cfg(cfg_),
    rrc_ue_du_proc_notifier(rrc_ue_du_proc_notif_),
    nas_notifier(nas_notif_),
    ngap_ctrl_notifier(ngap_ctrl_notif_),
    rrc_du_cu_cp_notifier(rrc_du_cu_cp_notifier_)
  {
  }
  const rrc_cfg_t&                    cfg;
  rrc_ue_du_processor_notifier&       rrc_ue_du_proc_notifier;
  rrc_ue_nas_notifier&                nas_notifier;
  rrc_ue_control_notifier&            ngap_ctrl_notifier;
  rrc_du_measurement_config_notifier& rrc_du_cu_cp_notifier;
};

/// Create an instance of an RRC entity
std::unique_ptr<rrc_du_interface> create_rrc_du(const rrc_du_creation_message& msg);

} // namespace srs_cu_cp

} // namespace srsran
