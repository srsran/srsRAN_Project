/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rlc_metrics_plotter_json.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

namespace {

/// UE container metrics.
DECLARE_METRIC("ue_id", metric_ue_id, uint16_t, "");
DECLARE_METRIC("drb_id", metric_drb_id, uint16_t, "");
DECLARE_METRIC_SET("drb_container", mset_drb_container, metric_ue_id);

/// Metrics root object.
DECLARE_METRIC("timestamp", metric_timestamp_tag, double, "");
DECLARE_METRIC_LIST("drb_list", mlist_drbs, std::vector<mset_drb_container>);

/// Metrics context.
using metric_context_t = srslog::build_context_type<metric_timestamp_tag, mlist_drbs>;

} // namespace

/// Returns the current time in seconds with ms precision since UNIX epoch.
static double get_time_stamp()
{
  auto tp = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(tp).count() * 1e-3;
}

void rlc_metrics_plotter_json::report_metrics(const rlc_metrics& drb)
{
  metric_context_t ctx("JSON RLC Metrics");

  ctx.get<mlist_drbs>().emplace_back();
  auto& output = ctx.get<mlist_drbs>().back();

  output.write<metric_ue_id>(drb.ue_index);

  // Log the context.
  ctx.write<metric_timestamp_tag>(get_time_stamp());
  log_chan(ctx);
}
