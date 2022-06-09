/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp.h"
#include "srsgnb/cu_cp/cu_cp_manager_factory.h"
#include "srsgnb/f1_interface/f1ap_cu_factory.h"

using namespace srsgnb;
using namespace srs_cu_cp;

void assert_cu_cp_configuration_valid(const cu_cp_configuration& cfg)
{
  srsran_assert(cfg.cu_executor != nullptr, "Invalid CU-CP executor");
}

cu_cp::cu_cp(const cu_cp_configuration& config_) : cfg(config_)
{
  assert_cu_cp_configuration_valid(cfg);

  // Create layers
  f1ap = create_f1ap(f1ap_ev_notifier);
  manager = srs_cu_cp::create_cu_cp_manager(timers, *cfg.cu_executor);

  // connect event notifier to layers
  f1ap_ev_notifier.connect(*manager);
}

cu_cp::~cu_cp()
{
  stop();
}

void cu_cp::start() {}

void cu_cp::stop() {}

size_t cu_cp::get_nof_dus()
{
  return manager->get_nof_dus();
}

f1c_message_handler& cu_cp::get_f1c_message_handler()
{
  return *f1ap;
}
