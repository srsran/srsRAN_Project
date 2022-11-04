/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_impl.h"

using namespace srsgnb;
using namespace srs_cu_up;

void assert_cu_up_configuration_valid(const cu_up_configuration& cfg)
{
  srsgnb_assert(cfg.cu_up_executor != nullptr, "Invalid CU-UP executor");
  srsgnb_assert(cfg.e1_notifier != nullptr, "Invalid E1 notifier");
}

cu_up::cu_up(const cu_up_configuration& config_) : cfg(config_), main_ctrl_loop(128)
{
  assert_cu_up_configuration_valid(cfg);

  // Create layers
  // .. todo ..

  // connect event notifier to layers
  // f1c_ev_notifier.connect_cu_cp(*this);
}

e1_message_handler& cu_up::get_e1_message_handler()
{
  return *e1;
}

void cu_up::on_e1_connection_establish()
{
  // handle event appropiatly
}

void cu_up::on_e1_connection_drop()
{
  // handle event appropiatly
}

// private