/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rrc_config.h"
#include "rrc_ue_config.h"
#include "srsran/asn1/rrc_nr/rrc_nr.h"
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
  rrc_du_creation_message(const rrc_cfg_t&              cfg_,
                          rrc_ue_du_processor_notifier& rrc_ue_du_proc_notif_,
                          rrc_ue_nas_notifier&          nas_notif_,
                          rrc_ue_control_notifier&      ngap_ctrl_notif_) :
    cfg(cfg_),
    rrc_ue_du_proc_notifier(rrc_ue_du_proc_notif_),
    nas_notifier(nas_notif_),
    ngap_ctrl_notifier(ngap_ctrl_notif_)
  {
  }
  const rrc_cfg_t&              cfg;
  rrc_ue_du_processor_notifier& rrc_ue_du_proc_notifier;
  rrc_ue_nas_notifier&          nas_notifier;
  rrc_ue_control_notifier&      ngap_ctrl_notifier;
};

/// Create an instance of an RRC entity
std::unique_ptr<rrc_du_interface> create_rrc_du(const rrc_du_creation_message& msg);

} // namespace srs_cu_cp

} // namespace srsran
