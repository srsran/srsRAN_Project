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

#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/cu_cp/ue_context.h"
#include "srsgnb/rrc/rrc_ue_entity_interface.h"

namespace srsgnb {

/// Reusable notifier class that a) stores the received msg for test inspection and b)
/// calls the registered msg handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_rrc_ue_nas_notifier : public srs_cu_cp::rrc_ue_nas_notifier
{
public:
  // FIXME: Add handler when ngap exists
  dummy_rrc_ue_nas_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  void on_initial_ue_message(const srs_cu_cp::initial_ue_message& msg) override
  {
    logger.info("Received Initial UE message");
  };

  void on_ul_nas_transport_message(const srs_cu_cp::ul_nas_transport_message& msg) override
  {
    logger.info("Received UL NAS transport message");
  };

private:
  srslog::basic_logger& logger;
};

} // namespace srsgnb
