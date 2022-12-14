/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rrc_config.h"
#include "rrc_ue_config.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/rrc/rrc.h"
#include "srsgnb/rrc/rrc_du.h"
#include "srsgnb/rrc/rrc_ue.h"
#include <map>
#include <memory>

namespace srsgnb {

namespace srs_cu_cp {

struct ue_context;

struct rrc_du_creation_message {
  rrc_du_creation_message(const rrc_cfg_t&              cfg_,
                          rrc_ue_du_processor_notifier& rrc_ue_du_proc_notif_,
                          rrc_ue_e1_control_notifier&   rrc_ue_e1_ctrl_notif_,
                          rrc_ue_nas_notifier&          nas_notif_,
                          rrc_ue_control_notifier&      ngc_ctrl_notif_) :
    cfg(cfg_),
    rrc_ue_du_proc_notifier(rrc_ue_du_proc_notif_),
    rrc_ue_e1_ctrl_notifier(rrc_ue_e1_ctrl_notif_),
    nas_notifier(nas_notif_),
    ngc_ctrl_notifier(ngc_ctrl_notif_)

  {
  }
  const rrc_cfg_t&              cfg;
  rrc_ue_du_processor_notifier& rrc_ue_du_proc_notifier;
  rrc_ue_e1_control_notifier&   rrc_ue_e1_ctrl_notifier;
  rrc_ue_nas_notifier&          nas_notifier;
  rrc_ue_control_notifier&      ngc_ctrl_notifier;
};

/// Create an instance of an RRC entity
std::unique_ptr<rrc_du_interface> create_rrc_du(const rrc_du_creation_message& msg);

} // namespace srs_cu_cp

} // namespace srsgnb
