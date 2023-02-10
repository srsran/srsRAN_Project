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

#include "srsgnb/cu_cp/cu_up_processor.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between E1 and CU-UP processor
class e1_cu_up_processor_adapter : public e1_cu_up_processor_notifier
{
public:
  void connect_cu_up_processor(cu_up_processor_e1_interface& cu_up_processor_e1_)
  {
    cu_up_e1_handler = &cu_up_processor_e1_;
  }

  void on_cu_up_e1_setup_request_received(const cu_up_e1_setup_request& msg) override
  {
    srsgnb_assert(cu_up_e1_handler != nullptr, "E1AP handler must not be nullptr");
    cu_up_e1_handler->handle_cu_up_e1_setup_request(msg);
  }

private:
  cu_up_processor_e1_interface* cu_up_e1_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsgnb
