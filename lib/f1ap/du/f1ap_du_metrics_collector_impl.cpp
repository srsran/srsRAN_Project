/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "f1ap_du_metrics_collector_impl.h"

using namespace srsran;
using namespace srs_du;

f1ap_metrics_collector_impl::f1ap_metrics_collector_impl(bool enabled) : enabled_flag(enabled) {}

void f1ap_metrics_collector_impl::collect_metrics_report(f1ap_metrics_report& report)
{
  report = {};
  if (not enabled_flag) {
    return;
  }

  // Prepare report.
  report.nof_rx_pdus = data.nof_rx_pdus;
  for (const auto& u : ues) {
    report.ues.push_back(u);
  }

  // Reset report data.
  data = {};
}

void f1ap_metrics_collector_impl::on_ue_creation(du_ue_index_t ue_index, gnb_du_ue_f1ap_id_t du_ue_id)
{
  if (not enabled_flag) {
    return;
  }

  ues.emplace(ue_index, f1ap_metrics_report::ue_metrics{ue_index, du_ue_id});
}

void f1ap_metrics_collector_impl::on_ue_removal(du_ue_index_t ue_index)
{
  if (not enabled_flag) {
    return;
  }

  ues.erase(ue_index);
}

void f1ap_metrics_collector_impl::on_rx_pdu(const f1ap_message& msg)
{
  if (not enabled_flag) {
    return;
  }

  data.nof_rx_pdus++;
}
