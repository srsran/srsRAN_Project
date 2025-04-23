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

#pragma once

#include "srsran/adt/slotted_vector.h"
#include "srsran/f1ap/du/f1ap_du_metrics_collector.h"

namespace srsran {

struct f1ap_message;

namespace srs_du {

/// Collector of F1AP metrics.
class f1ap_metrics_collector_impl final : public f1ap_metrics_collector
{
public:
  f1ap_metrics_collector_impl(bool enabled);

  void collect_metrics_report(f1ap_metrics_report& report) override;

  /// Called on every UE creation in the F1AP.
  void on_ue_creation(du_ue_index_t ue_index, gnb_du_ue_f1ap_id_t du_ue_id);

  /// Called on every UE removal in the F1AP.
  void on_ue_removal(du_ue_index_t ue_index);

  /// Called on every received PDU from the F1-C.
  void on_rx_pdu(const f1ap_message& msg);

private:
  struct non_persistent_common_data {
    unsigned nof_rx_pdus = 0;
  };

  bool enabled_flag;

  non_persistent_common_data                                        data;
  slotted_id_vector<du_ue_index_t, f1ap_metrics_report::ue_metrics> ues;
};

} // namespace srs_du
} // namespace srsran
