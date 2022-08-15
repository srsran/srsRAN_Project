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
#include "srsgnb/ngap/ngap.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Reusable notifier class that a) stores the received msg for test inspection and b)
/// calls the registered msg handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_ngap_amf_notifier : public ng_message_notifier
{
public:
  // FIXME: Add handler when ngap exists
  dummy_ngap_amf_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  void on_new_message(const ngap_msg& msg) override
  {
    logger.info("Received NGAP message");
    last_ngap_msg = msg;
  };

  ngap_msg last_ngap_msg;

private:
  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsgnb