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
#include "srsgnb/ngap/ngap.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between NGAP and AMF
class ngap_amf_adapter : public ngc_message_notifier
{
public:
  void on_new_message(const ngc_message& msg) override{
      // TODO: Add network socket transmit messages to AMF
  };
};

} // namespace srs_cu_cp
} // namespace srsgnb
