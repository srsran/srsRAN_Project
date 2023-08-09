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

e2sm_kpm_du_meas_provider_impl::e2sm_kpm_du_meas_provider_impl(srslog::basic_logger&    logger_,
                                                               e2_du_metrics_interface& du_metrics_interface_) :
  logger(srslog::fetch_basic_logger("E2SM-KPM")), du_metrics_interface(du_metrics_interface_)
{
}

bool e2sm_kpm_du_meas_provider_impl::check_measurement_name(meas_type_c meas_type, const char* meas)
{
  if (strcmp(meas_type.meas_name().to_string().c_str(), meas) == 0) {
    return true;
  }
  return false;
}

bool e2sm_kpm_du_meas_provider_impl::get_meas_data(const asn1::e2sm_kpm::meas_type_c&               meas_type,
                                                   const asn1::e2sm_kpm::label_info_list_l          label_info_list,
                                                   const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
                                                   const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
                                                   std::vector<asn1::e2sm_kpm::meas_record_item_c>& items)
{
  scheduler_ue_metrics ue_metrics = {};
  du_metrics_interface.get_metrics(ue_metrics);

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