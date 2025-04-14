/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
