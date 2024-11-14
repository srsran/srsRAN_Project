/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/gtpu/gtpu_demux.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>

namespace srsran {

struct gtpu_demux_creation_request {
  gtpu_demux_cfg_t cfg       = {};
  dlt_pcap*        gtpu_pcap = nullptr;
};

/// Creates an instance of an GTP-U demux object.
std::unique_ptr<gtpu_demux> create_gtpu_demux(const gtpu_demux_creation_request& msg);

} // namespace srsran
