/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_H
#define SRSGNB_CU_CP_H

#include "adapters.h"
#include "srsgnb/cu_cp/cu_cp_configuration.h"
#include "srsgnb/f1_interface/f1ap_cu.h"
#include "srsgnb/support/executors/task_worker.h"
#include <memory>

namespace srsgnb {

class cu_cp
{
public:
  explicit cu_cp(const cu_cp_configuration& cfg_);
  ~cu_cp();

  void start();
  void stop();

  f1c_message_handler& get_f1c_message_handler();

private:
  cu_cp_configuration cfg;

  std::unique_ptr<f1_cu_interface> f1ap;

  cu_manager_f1ap_event_indicator f1ap_ev_notifier;
};

} // namespace srsgnb

#endif // SRSGNB_CU_CP_H
