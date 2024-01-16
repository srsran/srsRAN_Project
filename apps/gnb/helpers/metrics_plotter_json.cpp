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

#include "metrics_plotter_json.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

namespace {

/// UE container metrics.
DECLARE_METRIC("pci", metric_pci, pci_t, "");
DECLARE_METRIC("rnti", metric_rnti, uint16_t, "");
DECLARE_METRIC("cqi", metric_cqi, uint8_t, "");
DECLARE_METRIC("ri", metric_ri, uint8_t, "");
DECLARE_METRIC("dl_mcs", metric_dl_mcs, uint8_t, "");
DECLARE_METRIC("dl_brate", metric_dl_brate, double, "");
DECLARE_METRIC("dl_nof_ok", metric_dl_nof_ok, unsigned, "");
DECLARE_METRIC("dl_nof_nok", metric_dl_nof_nok, unsigned, "");
DECLARE_METRIC("dl_bs", metric_dl_bs, unsigned, "");
DECLARE_METRIC("pusch_snr_db", metric_pusch_snr_db, float, "");
DECLARE_METRIC("ul_mcs", metric_ul_mcs, uint8_t, "");
DECLARE_METRIC("ul_brate", metric_ul_brate, double, "");
DECLARE_METRIC("ul_nof_ok", metric_ul_nof_ok, unsigned, "");
DECLARE_METRIC("ul_nof_nok", metric_ul_nof_nok, unsigned, "");
DECLARE_METRIC("bsr", metric_bsr, unsigned, "");
DECLARE_METRIC_SET("ue_container",
                   mset_ue_container,
                   metric_pci,
                   metric_rnti,
                   metric_cqi,
                   metric_ri,
                   metric_dl_mcs,
                   metric_dl_brate,
                   metric_dl_nof_ok,
                   metric_dl_nof_nok,
                   metric_dl_bs,
                   metric_pusch_snr_db,
                   metric_ul_mcs,
                   metric_ul_brate,
                   metric_ul_nof_ok,
                   metric_ul_nof_nok,
                   metric_bsr);

/// Metrics root object.
DECLARE_METRIC("timestamp", metric_timestamp_tag, double, "");
DECLARE_METRIC_LIST("ue_list", mlist_ues, std::vector<mset_ue_container>);

/// Metrics context.
using metric_context_t = srslog::build_context_type<metric_timestamp_tag, mlist_ues>;

} // namespace

/// Returns the current time in seconds with ms precision since UNIX epoch.
static double get_time_stamp()
{
  auto tp = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(tp).count() * 1e-3;
}

void metrics_plotter_json::report_metrics(span<const scheduler_ue_metrics> ue_metrics)
{
  metric_context_t ctx("JSON Metrics");

  for (const auto& ue : ue_metrics) {
    ctx.get<mlist_ues>().emplace_back();
    auto& output = ctx.get<mlist_ues>().back();

    output.write<metric_pci>(ue.pci);
    output.write<metric_rnti>(to_value(ue.rnti));
    if (ue.cqi) {
      output.write<metric_cqi>(ue.cqi);
    }
    output.write<metric_ri>(ue.ri);
    output.write<metric_dl_mcs>(ue.dl_mcs.to_uint());
    output.write<metric_dl_brate>(ue.dl_brate_kbps * 1e3);
    output.write<metric_dl_nof_ok>(ue.dl_nof_ok);
    output.write<metric_dl_nof_nok>(ue.dl_nof_nok);
    output.write<metric_dl_bs>(ue.dl_bs);
    if (!std::isnan(ue.pusch_snr_db) && !iszero(ue.pusch_snr_db)) {
      output.write<metric_pusch_snr_db>(clamp(ue.pusch_snr_db, -99.9f, 99.9f));
    }
    output.write<metric_ul_mcs>(ue.ul_mcs.to_uint());
    output.write<metric_ul_brate>(ue.ul_brate_kbps * 1e3);
    output.write<metric_ul_nof_ok>(ue.ul_nof_ok);
    output.write<metric_ul_nof_nok>(ue.ul_nof_nok);
    output.write<metric_bsr>(ue.bsr);
  }

  // Log the context.
  ctx.write<metric_timestamp_tag>(get_time_stamp());
  log_chan(ctx);
}
