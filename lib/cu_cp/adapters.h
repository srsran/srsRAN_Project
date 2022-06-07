/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_ADAPTERS_H
#define SRSGNB_ADAPTERS_H

#include "srsgnb/cu_cp/cu_cp_manager.h"
#include "srsgnb/f1_interface/f1ap_cu.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
namespace srs_cu_cp {

class cu_cp_manager_f1ap_event_indicator : public f1ap_message_notifier
{
public:
  void on_f1_setup_request_received(const f1_setup_request_message& msg) override
  {
    f1ap_setup_handler->handle_f1_setup_request(msg);
  }

private:
  cu_cp_manager_f1ap_setup_handler* f1ap_setup_handler;
};

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_ADAPTERS_H
