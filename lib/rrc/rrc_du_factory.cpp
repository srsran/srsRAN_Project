/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/rrc/rrc_du_factory.h"
#include "rrc_du_impl.h"
#include "ue/rrc_ue_impl.h"

using namespace srsgnb;
using namespace srs_cu_cp;

std::unique_ptr<rrc_du_interface> srsgnb::srs_cu_cp::create_rrc_du(const rrc_du_creation_message& msg)
{
  return std::make_unique<rrc_du_impl>(msg.cfg, msg.rrc_ue_du_proc_notifier, msg.nas_notifier, msg.ngc_ctrl_notifier);
}