/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/adt/span.h"
#include "srsran/asn1/e2ap/e2sm.h"
#include "srsran/asn1/e2ap/e2sm_rc.h"
#include "srsran/asn1/rrc_nr/serving_cell.h"

namespace srsran {

struct ric_control_config {
  uint64_t           ue_id;
  optional<unsigned> num_harq_processes;
  optional<unsigned> num_harq_retransmissions;
  optional<int>      max_prb_idx;
  optional<int>      min_prb_idx;
};

class e2sm_rc_param_configurator
{
public:
  virtual async_task<ric_control_config> configure_scheduler(ric_control_config reconf) = 0;
  virtual ~e2sm_rc_param_configurator()                                                 = default;
};

} // namespace srsran
