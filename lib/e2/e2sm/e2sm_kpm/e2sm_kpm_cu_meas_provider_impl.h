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

#include "e2sm_kpm_metric_defs.h"
#include "e2sm_kpm_utils.h"
#include "srsran/adt/optional.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2sm/e2sm_kpm_ies.h"
#include "srsran/e2/e2_cu.h"
#include "srsran/e2/e2sm/e2sm.h"
#include "srsran/e2/e2sm/e2sm_kpm.h"
#include <map>
#include <numeric>

namespace srsran {

class e2sm_kpm_cu_meas_provider_impl : public e2sm_kpm_meas_provider, public e2_cu_metrics_notifier
{
public:
  // constructor takes logger as argument
  e2sm_kpm_cu_meas_provider_impl();

  ~e2sm_kpm_cu_meas_provider_impl() = default;

  /// e2sm_kpm_meas_provider functions.
  std::vector<std::string> get_supported_metric_names(e2sm_kpm_metric_level_enum level) override;

  bool is_cell_supported(const asn1::e2sm::cgi_c& cell_global_id) override;

  bool is_ue_supported(const asn1::e2sm::ue_id_c& ueid) override;

  bool is_test_cond_supported(const asn1::e2sm::test_cond_type_c& test_cond_type) override;

  bool is_metric_supported(const asn1::e2sm::meas_type_c&   meas_type,
                           const asn1::e2sm::meas_label_s&  label,
                           const e2sm_kpm_metric_level_enum level,
                           const bool&                      cell_scope) override;

  bool get_ues_matching_test_conditions(const asn1::e2sm::matching_cond_list_l& matching_cond_list,
                                        std::vector<asn1::e2sm::ue_id_c>&       ues) override;

  bool get_ues_matching_test_conditions(const asn1::e2sm::matching_ue_cond_per_sub_list_l& matching_ue_cond_list,
                                        std::vector<asn1::e2sm::ue_id_c>&                  ues) override;

  bool get_meas_data(const asn1::e2sm::meas_type_c&               meas_type,
                     const asn1::e2sm::label_info_list_l          label_info_list,
                     const std::vector<asn1::e2sm::ue_id_c>&      ues,
                     const std::optional<asn1::e2sm::cgi_c>       cell_global_id,
                     std::vector<asn1::e2sm::meas_record_item_c>& items) override;

private:
  typedef bool(metric_meas_getter_func_t)(const asn1::e2sm::label_info_list_l          label_info_list,
                                          const std::vector<asn1::e2sm::ue_id_c>&      ues,
                                          const std::optional<asn1::e2sm::cgi_c>       cell_global_id,
                                          std::vector<asn1::e2sm::meas_record_item_c>& items);

  typedef metric_meas_getter_func_t(e2sm_kpm_cu_meas_provider_impl::*metric_meas_getter_func_ptr);

  struct e2sm_kpm_supported_metric_t {
    uint32_t                    supported_labels;
    uint32_t                    supported_levels;
    bool                        cell_scope_supported;
    metric_meas_getter_func_ptr func;
  };

  bool check_e2sm_kpm_metrics_definitions(span<const e2sm_kpm_metric_t> metrics_defs);

  // Helper functions.
  float bytes_to_kbits(float value);

  // Measurement getter functions.
  metric_meas_getter_func_t get_pdcp_reordering_delay_ul;

  srslog::basic_logger& logger;

  std::map<std::string, e2sm_kpm_supported_metric_t> supported_metrics;
};

} // namespace srsran