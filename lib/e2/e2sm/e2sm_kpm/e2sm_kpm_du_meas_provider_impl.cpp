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

e2sm_kpm_du_meas_provider_impl::e2sm_kpm_du_meas_provider_impl(srs_du::f1ap_ue_id_translator& f1ap_ue_id_translator_) :
  logger(srslog::fetch_basic_logger("E2SM-KPM")), f1ap_ue_id_provider(f1ap_ue_id_translator_)
{
  // Array of supported metrics.
  supported_metrics.emplace(
      "CQI", e2sm_kpm_supported_metric_t{NO_LABEL, ALL_LEVELS, false, &e2sm_kpm_du_meas_provider_impl::get_cqi});
  supported_metrics.emplace(
      "RSRP", e2sm_kpm_supported_metric_t{NO_LABEL, ALL_LEVELS, false, &e2sm_kpm_du_meas_provider_impl::get_rsrp});
  supported_metrics.emplace(
      "RSRQ", e2sm_kpm_supported_metric_t{NO_LABEL, ALL_LEVELS, false, &e2sm_kpm_du_meas_provider_impl::get_rsrq});
  supported_metrics.emplace(
      "DRB.RlcPacketDropRateDl",
      e2sm_kpm_supported_metric_t{
          NO_LABEL, UE_LEVEL, false, &e2sm_kpm_du_meas_provider_impl::get_drb_rlc_packet_drop_rate_dl});
  supported_metrics.emplace(
      "DRB.RlcSduTransmittedVolumeDL",
      e2sm_kpm_supported_metric_t{
          NO_LABEL, UE_LEVEL, false, &e2sm_kpm_du_meas_provider_impl::get_drb_rlc_sdu_transmitted_volume_dl});
  supported_metrics.emplace(
      "DRB.RlcSduTransmittedVolumeUL",
      e2sm_kpm_supported_metric_t{
          NO_LABEL, UE_LEVEL, false, &e2sm_kpm_du_meas_provider_impl::get_drb_rlc_sdu_transmitted_volume_ul});

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
  logger.debug("Received RLC metrics: ue={} {}", metrics.ue_index, metrics.rb_id.get_drb_id());

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
  std::vector<std::string> metrics;
  for (auto& m : supported_metrics) {
    if ((level & E2_NODE_LEVEL) and (m.second.supported_levels & E2_NODE_LEVEL)) {
      metrics.push_back(m.first);
    } else if ((level & UE_LEVEL) and (m.second.supported_levels & UE_LEVEL)) {
      metrics.push_back(m.first);
    }
  }
  return metrics;
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
    if (strcmp(meas_type.meas_name().to_string().c_str(), metric.first.c_str()) == 0) {
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
  metric_meas_getter_func_ptr metric_meas_getter_func;
  auto                        it = supported_metrics.find(meas_type.meas_name().to_string().c_str());
  if (it == supported_metrics.end()) {
    logger.debug("Metric {} not supported", meas_type.meas_name().to_string());
    return false;
  }
  metric_meas_getter_func = it->second.func;
  srsran_assert(metric_meas_getter_func != nullptr, "Metric getter function cannot be empty.");
  return (this->*metric_meas_getter_func)(label_info_list, ues, cell_global_id, items);
}

bool e2sm_kpm_du_meas_provider_impl::get_cqi(const asn1::e2sm_kpm::label_info_list_l          label_info_list,
                                             const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
                                             const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
                                             std::vector<asn1::e2sm_kpm::meas_record_item_c>& items)
{
  bool                 meas_collected = false;
  scheduler_ue_metrics ue_metrics     = last_ue_metrics[0];

  meas_record_item_c meas_record_item;
  meas_record_item.set_integer() = (int)ue_metrics.cqi;
  items.push_back(meas_record_item);
  meas_collected = true;

  return meas_collected;
}

bool e2sm_kpm_du_meas_provider_impl::get_rsrp(const asn1::e2sm_kpm::label_info_list_l          label_info_list,
                                              const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
                                              const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
                                              std::vector<asn1::e2sm_kpm::meas_record_item_c>& items)
{
  bool                 meas_collected = false;
  scheduler_ue_metrics ue_metrics     = last_ue_metrics[0];

  meas_record_item_c meas_record_item;
  meas_record_item.set_integer() = (int)ue_metrics.pusch_snr_db;
  items.push_back(meas_record_item);
  meas_collected = true;

  return meas_collected;
}

bool e2sm_kpm_du_meas_provider_impl::get_rsrq(const asn1::e2sm_kpm::label_info_list_l          label_info_list,
                                              const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
                                              const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
                                              std::vector<asn1::e2sm_kpm::meas_record_item_c>& items)
{
  bool                 meas_collected = false;
  scheduler_ue_metrics ue_metrics     = last_ue_metrics[0];

  meas_record_item_c meas_record_item;
  meas_record_item.set_integer() = (int)ue_metrics.pusch_snr_db;
  items.push_back(meas_record_item);
  meas_collected = true;

  return meas_collected;
}

bool e2sm_kpm_du_meas_provider_impl::get_drb_rlc_packet_drop_rate_dl(
    const asn1::e2sm_kpm::label_info_list_l          label_info_list,
    const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
    const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
    std::vector<asn1::e2sm_kpm::meas_record_item_c>& items)
{
  bool meas_collected = false;

  if ((label_info_list.size() > 1 or
       (label_info_list.size() == 1 and not label_info_list[0].meas_label.no_label_present))) {
    logger.debug("Metric: DRB.RlcPacketDropRateDl supports only NO_LABEL label.");
  }

  if (cell_global_id.has_value()) {
    logger.debug("Metric: DRB.RlcPacketDropRateDl does not support cell_global_id filter.");
    return false;
  }

  if (ues.size() == 0) {
    logger.debug("Metric: DRB.RlcPacketDropRateDl supports only UE_level measurements.");
    return false;
  }

  for (auto& ue : ues) {
    meas_record_item_c  meas_record_item;
    gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(ue.gnb_du_ueid().gnb_cu_ue_f1_ap_id);
    uint32_t            ue_idx            = f1ap_ue_id_provider.get_ue_index(gnb_cu_ue_f1ap_id);
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
  return meas_collected;
}

bool e2sm_kpm_du_meas_provider_impl::get_drb_rlc_sdu_transmitted_volume_dl(
    const asn1::e2sm_kpm::label_info_list_l          label_info_list,
    const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
    const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
    std::vector<asn1::e2sm_kpm::meas_record_item_c>& items)
{
  bool meas_collected = false;

  if ((label_info_list.size() > 1 or
       (label_info_list.size() == 1 and not label_info_list[0].meas_label.no_label_present))) {
    logger.debug("Metric: DRB.RlcSduTransmittedVolumeDL supports only NO_LABEL label.");
  }

  if (cell_global_id.has_value()) {
    logger.debug("Metric: DRB.RlcSduTransmittedVolumeDL does not support cell_global_id filter.");
    return false;
  }

  if (ues.size() == 0) {
    logger.debug("Metric: DRB.RlcSduTransmittedVolumeDL supports only UE_level measurements.");
    return false;
  }

  for (auto& ue : ues) {
    meas_record_item_c  meas_record_item;
    gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(ue.gnb_du_ueid().gnb_cu_ue_f1_ap_id);
    uint32_t            ue_idx            = f1ap_ue_id_provider.get_ue_index(gnb_cu_ue_f1ap_id);
    if (ue_idx > ue_aggr_rlc_metrics.size()) {
      meas_record_item.set_no_value();
      continue;
    }
    meas_record_item.set_integer() = ue_aggr_rlc_metrics[ue_idx].tx.num_sdu_bytes * 8 / 1000; // unit is kbit
    items.push_back(meas_record_item);
    meas_collected = true;
  }
  return meas_collected;
}

bool e2sm_kpm_du_meas_provider_impl::get_drb_rlc_sdu_transmitted_volume_ul(
    const asn1::e2sm_kpm::label_info_list_l          label_info_list,
    const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
    const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
    std::vector<asn1::e2sm_kpm::meas_record_item_c>& items)
{
  bool meas_collected = false;

  if ((label_info_list.size() > 1 or
       (label_info_list.size() == 1 and not label_info_list[0].meas_label.no_label_present))) {
    logger.debug("Metric: DRB.RlcSduTransmittedVolumeUL supports only NO_LABEL label.");
  }

  if (cell_global_id.has_value()) {
    logger.debug("Metric: DRB.RlcSduTransmittedVolumeUL does not support cell_global_id filter.");
    return false;
  }

  if (ues.size() == 0) {
    logger.debug("Metric: DRB.RlcSduTransmittedVolumeUL supports only UE_level measurements.");
    return false;
  }

  for (auto& ue : ues) {
    meas_record_item_c  meas_record_item;
    gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(ue.gnb_du_ueid().gnb_cu_ue_f1_ap_id);
    uint32_t            ue_idx            = f1ap_ue_id_provider.get_ue_index(gnb_cu_ue_f1ap_id);
    if (ue_idx > ue_aggr_rlc_metrics.size()) {
      meas_record_item.set_no_value();
      continue;
    }
    meas_record_item.set_integer() = ue_aggr_rlc_metrics[ue_idx].rx.num_sdu_bytes * 8 / 1000; // unit is kbit
    items.push_back(meas_record_item);
    meas_collected = true;
  }
  return meas_collected;
}
