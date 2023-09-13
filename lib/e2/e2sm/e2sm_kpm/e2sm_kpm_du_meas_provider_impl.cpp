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
  // Array of supported metrics in string format.
  supported_metrics = {"CQI",
                       "RSRP",
                       "RSRQ",
                       "DRB.RlcPacketDropRateDl",
                       "DRB.RlcSduTransmittedVolumeDL",
                       "DRB.RlcSduTransmittedVolumeUL"};
}

bool e2sm_kpm_du_meas_provider_impl::check_measurement_family(const meas_type_c meas_type, const char* family_name)
{
  std::string family_name_str = family_name;
  family_name_str += ".";
  if (meas_type.meas_name().to_string().find(family_name_str) != std::string::npos) {
    return true;
  }
  return false;
}

bool e2sm_kpm_du_meas_provider_impl::check_measurement_name(const meas_type_c meas_type, const char* meas)
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

void e2sm_kpm_du_meas_provider_impl::report_metrics(const rlc_metrics& metrics)
{
  uint32_t bearer_id = drb_id_to_uint(metrics.rb_id.get_drb_id());
  logger.debug("Received RLC metrics: UE_idx: {}, RB_id: {} (as int: {})",
               metrics.ue_index,
               metrics.rb_id.get_drb_id(),
               bearer_id);

  if (metrics.ue_index >= ue_aggr_rlc_metrics.size()) {
    ue_aggr_rlc_metrics.resize(metrics.ue_index + 1);
  }

  if (bearer_id == 1) {
    // Reset aggregated RLC metrics when metrics for bearer_id = 0 are received.
    ue_aggr_rlc_metrics[metrics.ue_index].rx.num_lost_pdus        = metrics.rx.num_lost_pdus;
    ue_aggr_rlc_metrics[metrics.ue_index].rx.num_malformed_pdus   = metrics.rx.num_malformed_pdus;
    ue_aggr_rlc_metrics[metrics.ue_index].rx.num_sdus             = metrics.rx.num_sdus;
    ue_aggr_rlc_metrics[metrics.ue_index].rx.num_sdu_bytes        = metrics.rx.num_sdu_bytes;
    ue_aggr_rlc_metrics[metrics.ue_index].rx.num_pdus             = metrics.rx.num_pdus;
    ue_aggr_rlc_metrics[metrics.ue_index].rx.num_pdu_bytes        = metrics.rx.num_pdu_bytes;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_sdus             = metrics.tx.num_sdus;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_sdu_bytes        = metrics.tx.num_sdu_bytes;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_dropped_sdus     = metrics.tx.num_dropped_sdus;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_discarded_sdus   = metrics.tx.num_discarded_sdus;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_discard_failures = metrics.tx.num_discard_failures;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_pdus             = metrics.tx.num_pdus;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_pdu_bytes        = metrics.tx.num_pdu_bytes;
  } else {
    // Otherwise, aggregate RLC metrics for each UE.
    ue_aggr_rlc_metrics[metrics.ue_index].rx.num_lost_pdus += metrics.rx.num_lost_pdus;
    ue_aggr_rlc_metrics[metrics.ue_index].rx.num_malformed_pdus += metrics.rx.num_malformed_pdus;
    ue_aggr_rlc_metrics[metrics.ue_index].rx.num_sdus += metrics.rx.num_sdus;
    ue_aggr_rlc_metrics[metrics.ue_index].rx.num_sdu_bytes += metrics.rx.num_sdu_bytes;
    ue_aggr_rlc_metrics[metrics.ue_index].rx.num_pdus += metrics.rx.num_pdus;
    ue_aggr_rlc_metrics[metrics.ue_index].rx.num_pdu_bytes += metrics.rx.num_pdu_bytes;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_sdus += metrics.tx.num_sdus;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_sdu_bytes += metrics.tx.num_sdu_bytes;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_dropped_sdus += metrics.tx.num_dropped_sdus;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_discarded_sdus += metrics.tx.num_discarded_sdus;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_discard_failures += metrics.tx.num_discard_failures;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_pdus += metrics.tx.num_pdus;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_pdu_bytes += metrics.tx.num_pdu_bytes;
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

bool e2sm_kpm_du_meas_provider_impl::get_ues_matching_test_conditions(
    const asn1::e2sm_kpm::matching_cond_list_l& matching_cond_list,
    std::vector<asn1::e2sm_kpm::ueid_c>&        ues)
{
  return false;
}

bool e2sm_kpm_du_meas_provider_impl::get_ues_matching_test_conditions(
    const asn1::e2sm_kpm::matching_ue_cond_per_sub_list_l& matching_ue_cond_list,
    std::vector<asn1::e2sm_kpm::ueid_c>&                   ues)
{
  return false;
}

bool e2sm_kpm_du_meas_provider_impl::get_meas_data(const asn1::e2sm_kpm::meas_type_c&               meas_type,
                                                   const asn1::e2sm_kpm::label_info_list_l          label_info_list,
                                                   const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
                                                   const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
                                                   std::vector<asn1::e2sm_kpm::meas_record_item_c>& items)
{
  if (check_measurement_family(meas_type, "DRB")) {
    return get_meas_data_drb_family(meas_type, label_info_list, ues, cell_global_id, items);
  }

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

bool e2sm_kpm_du_meas_provider_impl::get_meas_data_drb_family(
    const asn1::e2sm_kpm::meas_type_c&               meas_type,
    const asn1::e2sm_kpm::label_info_list_l          label_info_list,
    const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
    const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
    std::vector<asn1::e2sm_kpm::meas_record_item_c>& items)
{
  bool meas_collected = false;

  if (ues.size() == 0) {
    // TODO: support E2 level measurement (i.e., aggregated)
  }

  if (cell_global_id.has_value()) {
    // TODO: filter measurements by cell_id
  }

  if (label_info_list.size()) {
    // TODO: need to get measurements with present labels
  }

  if (check_measurement_name(meas_type, "DRB.RlcPacketDropRateDl")) {
    for (auto& ue : ues) {
      meas_record_item_c meas_record_item;
      uint32_t           ue_idx = ue.gnb_du_ueid().gnb_cu_ue_f1_ap_id;
      if (ue_idx > ue_aggr_rlc_metrics.size()) {
        meas_record_item.set_no_value();
        continue;
      }
      float drop_rate = 0;
      if (ue_aggr_rlc_metrics[ue_idx].tx.num_sdus) {
        uint32_t dropped_sdus = ue_aggr_rlc_metrics[ue_idx].tx.num_dropped_sdus +
                                ue_aggr_rlc_metrics[ue_idx].tx.num_discarded_sdus +
                                ue_aggr_rlc_metrics[ue_idx].tx.num_discard_failures;
        drop_rate = 1.0 * dropped_sdus / ue_aggr_rlc_metrics[ue_idx].tx.num_sdus;
      }
      uint32_t drop_rate_int         = drop_rate * 100;
      meas_record_item.set_integer() = drop_rate_int;
      items.push_back(meas_record_item);
      meas_collected = true;
    }
  } else if (check_measurement_name(meas_type, "DRB.RlcSduTransmittedVolumeDL")) {
    for (auto& ue : ues) {
      meas_record_item_c meas_record_item;
      uint32_t           ue_idx = ue.gnb_du_ueid().gnb_cu_ue_f1_ap_id;
      if (ue_idx > ue_aggr_rlc_metrics.size()) {
        meas_record_item.set_no_value();
        continue;
      }
      meas_record_item.set_integer() = ue_aggr_rlc_metrics[ue_idx].tx.num_sdu_bytes * 8 / 1000; // unit is kbit
      items.push_back(meas_record_item);
      meas_collected = true;
    }
  } else if (check_measurement_name(meas_type, "DRB.RlcSduTransmittedVolumeUL")) {
    for (auto& ue : ues) {
      meas_record_item_c meas_record_item;
      uint32_t           ue_idx = ue.gnb_du_ueid().gnb_cu_ue_f1_ap_id;
      if (ue_idx > ue_aggr_rlc_metrics.size()) {
        meas_record_item.set_no_value();
        continue;
      }
      meas_record_item.set_integer() = ue_aggr_rlc_metrics[ue_idx].rx.num_sdu_bytes * 8 / 1000; // unit is kbit
      items.push_back(meas_record_item);
      meas_collected = true;
    }
  }
  return meas_collected;
}
