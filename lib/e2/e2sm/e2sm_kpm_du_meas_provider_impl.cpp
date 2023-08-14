/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2sm_kpm_du_meas_provider_impl.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm_kpm;
using namespace srsran;

e2sm_kpm_du_meas_provider_impl::e2sm_kpm_du_meas_provider_impl() : logger(srslog::fetch_basic_logger("E2SM-KPM"))
{
  // array of supported metrics in string format
  supported_metrics = {"CQI", "RSRP", "RSRQ"};
}

bool e2sm_kpm_du_meas_provider_impl::check_measurement_name(meas_type_c meas_type, const char* meas)
{
  if (strcmp(meas_type.meas_name().to_string().c_str(), meas) == 0) {
    return true;
  }
  return false;
}

void e2sm_kpm_du_meas_provider_impl::report_metrics(span<const scheduler_ue_metrics> ue_metrics)
{
  last_ue_metrics.clear();
  for (auto& ue_metric : ue_metrics) {
    last_ue_metrics.push_back(ue_metric);
  }
}

std::vector<std::string> e2sm_kpm_du_meas_provider_impl::get_supported_metric_names(e2sm_kpm_metric_level_enum level)
{
  return supported_metrics;
}

bool e2sm_kpm_du_meas_provider_impl::cell_supported(const asn1::e2sm_kpm::cgi_c& cell_global_id)
{
  // TODO: check if CELL is supported
  return true;
}

bool e2sm_kpm_du_meas_provider_impl::ue_supported(const asn1::e2sm_kpm::ueid_c& ueid)
{
  // TODO: check if UE is supported
  return true;
}

bool e2sm_kpm_du_meas_provider_impl::test_cond_supported(const asn1::e2sm_kpm::test_cond_type_c& test_cond_type)
{
  // TODO: check if test condition is supported
  return true;
}

bool e2sm_kpm_du_meas_provider_impl::metric_supported(const asn1::e2sm_kpm::meas_type_c&  meas_type,
                                                      const asn1::e2sm_kpm::meas_label_s& label,
                                                      const e2sm_kpm_metric_level_enum    level,
                                                      const bool&                         cell_scope)
{
  if (!label.no_label_present) {
    logger.debug("Currently only NO_LABEL metric supported");
    return false;
  }

  for (auto& metric : supported_metrics) {
    if (strcmp(meas_type.meas_name().to_string().c_str(), metric.c_str()) == 0) {
      return true;
    }
  }

  // TODO: check if metric supported with required label, level and cell_scope
  return false;
}

bool e2sm_kpm_du_meas_provider_impl::get_meas_data(const asn1::e2sm_kpm::meas_type_c&               meas_type,
                                                   const asn1::e2sm_kpm::label_info_list_l          label_info_list,
                                                   const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
                                                   const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
                                                   std::vector<asn1::e2sm_kpm::meas_record_item_c>& items)
{
  if (last_ue_metrics.size() == 0) {
    // TODO: handle this case correctly
    meas_record_item_c meas_record_item;
    meas_record_item.set_integer() = 0;
    items.push_back(meas_record_item);
    return false;
  }

  scheduler_ue_metrics ue_metrics = last_ue_metrics[0];

  if (cell_global_id.has_value()) {
    // TODO: need to find the proper cell
  }

  if (ues.size()) {
    // TODO: need to get measurements for the listed UEs
  }

  if (label_info_list.size()) {
    // TODO: need to get measurements with present labels
  }

  meas_record_item_c meas_record_item;
  if (check_measurement_name(meas_type, "CQI")) {
    meas_record_item.set_integer() = (int)ue_metrics.cqi;
  } else if (check_measurement_name(meas_type, "RSRP")) {
    meas_record_item.set_integer() = (int)ue_metrics.pusch_snr_db;
  } else if (check_measurement_name(meas_type, "RSRQ")) {
    meas_record_item.set_integer() = (int)ue_metrics.pusch_snr_db;
  }

  items.push_back(meas_record_item);
  return true;
}