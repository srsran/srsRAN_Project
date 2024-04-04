/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "rlc_metrics_plotter_json.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

namespace {

/// DRB container metrics.
DECLARE_METRIC("du_id", metric_du_id, uint32_t, "");
DECLARE_METRIC("ue_id", metric_ue_id, uint16_t, "");
DECLARE_METRIC("drb_id", metric_drb_id, uint8_t, "");

// TX specific metrics
DECLARE_METRIC("num_sdus", metric_tx_num_sdus, uint32_t, "");
DECLARE_METRIC("num_sdu_bytes", metric_tx_num_sdu_bytes, uint32_t, "");
DECLARE_METRIC("num_dropped_sdus", metric_tx_num_dropped_sdus, uint32_t, "");
DECLARE_METRIC("num_discarded_sdus", metric_tx_num_discarded_sdus, uint32_t, "");
DECLARE_METRIC("num_discard_failures", metric_tx_num_discard_failures, uint32_t, "");
DECLARE_METRIC("num_pdus", metric_tx_num_pdus, uint32_t, "");
DECLARE_METRIC("num_pdu_bytes", metric_tx_num_pdu_bytes, uint32_t, "");
DECLARE_METRIC_SET("tx",
                   mset_drb_tx_container,
                   metric_tx_num_sdus,
                   metric_tx_num_sdu_bytes,
                   metric_tx_num_dropped_sdus,
                   metric_tx_num_discarded_sdus,
                   metric_tx_num_discard_failures,
                   metric_tx_num_pdus,
                   metric_tx_num_pdu_bytes);

DECLARE_METRIC("num_sdus", metric_rx_num_sdus, uint32_t, "");
DECLARE_METRIC("num_sdu_bytes", metric_rx_num_sdu_bytes, uint32_t, "");
DECLARE_METRIC("num_pdus", metric_rx_num_pdus, uint32_t, "");
DECLARE_METRIC("num_pdu_bytes", metric_rx_num_pdu_bytes, uint32_t, "");
DECLARE_METRIC("num_lost_pdus", metric_rx_num_lost_pdus, uint32_t, "");
DECLARE_METRIC("num_malformed_pdus", metric_rx_num_malformed_pdus, uint32_t, "");
DECLARE_METRIC_SET("rx",
                   mset_drb_rx_container,
                   metric_rx_num_sdus,
                   metric_rx_num_sdu_bytes,
                   metric_rx_num_pdus,
                   metric_rx_num_pdu_bytes,
                   metric_rx_num_lost_pdus,
                   metric_rx_num_malformed_pdus);

DECLARE_METRIC_SET("drb",
                   mset_rlc_container,
                   metric_du_id,
                   metric_ue_id,
                   metric_drb_id,
                   mset_drb_tx_container,
                   mset_drb_rx_container);

/// Metrics root object.
DECLARE_METRIC("timestamp", metric_timestamp_tag, double, "");
DECLARE_METRIC_LIST("rlc_metrics", mlist_drbs, std::vector<mset_rlc_container>);

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

  output.write<metric_du_id>(drb.du_index);
  output.write<metric_ue_id>(drb.ue_index);
  output.write<metric_drb_id>(static_cast<uint8_t>(drb.rb_id.get_drb_id()));

  // TX metrics
  auto& tx_output = output.get<mset_drb_tx_container>();
  tx_output.write<metric_tx_num_sdus>(drb.tx.num_sdus);
  tx_output.write<metric_tx_num_sdu_bytes>(drb.tx.num_sdu_bytes);
  tx_output.write<metric_tx_num_dropped_sdus>(drb.tx.num_dropped_sdus);
  tx_output.write<metric_tx_num_discarded_sdus>(drb.tx.num_discarded_sdus);
  tx_output.write<metric_tx_num_discard_failures>(drb.tx.num_discarded_sdus);

  // RX metrics
  auto& rx_output = output.get<mset_drb_rx_container>();
  rx_output.write<metric_rx_num_sdus>(drb.rx.num_sdus);
  rx_output.write<metric_rx_num_sdu_bytes>(drb.rx.num_sdu_bytes);
  rx_output.write<metric_rx_num_pdus>(drb.rx.num_pdus);
  rx_output.write<metric_rx_num_pdu_bytes>(drb.rx.num_pdu_bytes);
  rx_output.write<metric_rx_num_lost_pdus>(drb.rx.num_lost_pdus);
  rx_output.write<metric_rx_num_malformed_pdus>(drb.rx.num_malformed_pdus);

  // Log the context.
  ctx.write<metric_timestamp_tag>(get_time_stamp());
  log_chan(ctx);
}
