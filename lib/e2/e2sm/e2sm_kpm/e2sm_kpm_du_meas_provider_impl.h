/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "e2sm_kpm_utils.h"
#include "srsran/adt/optional.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm.h"
#include "srsran/e2/e2sm/e2sm_kpm.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include <map>

namespace srsran {

class e2sm_kpm_du_meas_provider_impl : public e2sm_kpm_meas_provider, public e2_du_metrics_notifier
{
public:
  // constructor takes logger as argument
  e2sm_kpm_du_meas_provider_impl(srs_du::f1ap_ue_id_translator& f1ap_ue_id_translator);

  ~e2sm_kpm_du_meas_provider_impl() = default;

  /// scheduler_ue_metrics_notifier functions.
  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override;
  void report_metrics(const rlc_metrics& metrics) override;

  /// e2sm_kpm_meas_provider functions.
  std::vector<std::string> get_supported_metric_names(e2sm_kpm_metric_level_enum level) override;

  bool cell_supported(const asn1::e2sm_kpm::cgi_c& cell_global_id) override;

  bool ue_supported(const asn1::e2sm_kpm::ueid_c& ueid) override;

  bool test_cond_supported(const asn1::e2sm_kpm::test_cond_type_c& test_cond_type) override;

  bool metric_supported(const asn1::e2sm_kpm::meas_type_c&  meas_type,
                        const asn1::e2sm_kpm::meas_label_s& label,
                        const e2sm_kpm_metric_level_enum    level,
                        const bool&                         cell_scope) override;

  bool get_ues_matching_test_conditions(const asn1::e2sm_kpm::matching_cond_list_l& matching_cond_list,
                                        std::vector<asn1::e2sm_kpm::ueid_c>&        ues) override;

  bool get_ues_matching_test_conditions(const asn1::e2sm_kpm::matching_ue_cond_per_sub_list_l& matching_ue_cond_list,
                                        std::vector<asn1::e2sm_kpm::ueid_c>&                   ues) override;

  bool get_meas_data(const asn1::e2sm_kpm::meas_type_c&               meas_type,
                     const asn1::e2sm_kpm::label_info_list_l          label_info_list,
                     const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
                     const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
                     std::vector<asn1::e2sm_kpm::meas_record_item_c>& items) override;

private:
  bool check_measurement_family(const asn1::e2sm_kpm::meas_type_c meas_type, const char* family_name);
  bool check_measurement_name(const asn1::e2sm_kpm::meas_type_c meas_type, const char* meas);

  bool get_meas_data_drb_family(const asn1::e2sm_kpm::meas_type_c&               meas_type,
                                const asn1::e2sm_kpm::label_info_list_l          label_info_list,
                                const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
                                const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
                                std::vector<asn1::e2sm_kpm::meas_record_item_c>& items);

  srslog::basic_logger&             logger;
  srs_du::f1ap_ue_id_translator&    f1ap_ue_id_provider;
  std::vector<std::string>          supported_metrics;
  std::vector<scheduler_ue_metrics> last_ue_metrics;
  std::vector<rlc_metrics>          ue_aggr_rlc_metrics;
};

} // namespace srsran
