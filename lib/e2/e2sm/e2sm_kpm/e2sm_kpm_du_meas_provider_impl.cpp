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
      "DRB.PacketSuccessRateUlgNBUu",
      e2sm_kpm_supported_metric_t{
          NO_LABEL, E2_NODE_LEVEL | UE_LEVEL, true, &e2sm_kpm_du_meas_provider_impl::get_drb_ul_success_rate});
  supported_metrics.emplace(
      "DRB.UEThpDl",
      e2sm_kpm_supported_metric_t{
          NO_LABEL, E2_NODE_LEVEL | UE_LEVEL, true, &e2sm_kpm_du_meas_provider_impl::get_drb_dl_mean_throughput});
  supported_metrics.emplace(
      "DRB.UEThpUl",
      e2sm_kpm_supported_metric_t{
          NO_LABEL, E2_NODE_LEVEL | UE_LEVEL, true, &e2sm_kpm_du_meas_provider_impl::get_drb_ul_mean_throughput});
  supported_metrics.emplace(
      "DRB.RlcPacketDropRateDl",
      e2sm_kpm_supported_metric_t{
          NO_LABEL, ALL_LEVELS, true, &e2sm_kpm_du_meas_provider_impl::get_drb_rlc_packet_drop_rate_dl});
  supported_metrics.emplace(
      "DRB.RlcSduTransmittedVolumeDL",
      e2sm_kpm_supported_metric_t{
          NO_LABEL, ALL_LEVELS, true, &e2sm_kpm_du_meas_provider_impl::get_drb_rlc_sdu_transmitted_volume_dl});
  supported_metrics.emplace(
      "DRB.RlcSduTransmittedVolumeUL",
      e2sm_kpm_supported_metric_t{
          NO_LABEL, ALL_LEVELS, true, &e2sm_kpm_du_meas_provider_impl::get_drb_rlc_sdu_transmitted_volume_ul});

  // Check if the supported metrics are matching e2sm_kpm metrics definitions.
  check_e2sm_kpm_metrics_definitions(get_e2sm_kpm_28_552_metrics());
  check_e2sm_kpm_metrics_definitions(get_e2sm_kpm_oran_metrics());
}

bool e2sm_kpm_du_meas_provider_impl::check_e2sm_kpm_metrics_definitions(span<const e2sm_kpm_metric_t> metric_defs)
{
  std::string metric_name;
  auto        name_matches = [&metric_name](const e2sm_kpm_metric_t& x) {
    return (x.name == metric_name.c_str() or x.name == metric_name);
  };

  for (auto& supported_metric : supported_metrics) {
    metric_name = supported_metric.first;
    auto it     = std::find_if(metric_defs.begin(), metric_defs.end(), name_matches);
    if (it == metric_defs.end()) {
      continue;
    }

    if (supported_metric.second.supported_labels & ~(it->optional_labels | e2sm_kpm_label_enum::NO_LABEL)) {
      logger.debug("Wrong definition of the supported metric: \"{}\", labels cannot be supported.", metric_name);
    }

    if (supported_metric.second.supported_levels & ~it->optional_levels) {
      logger.debug("Wrong definition of the supported metric: \"{}\", level cannot be supported.", metric_name);
    }

    if (is_cell_id_required(*it) and not supported_metric.second.cell_scope_supported) {
      logger.debug("Wrong definition of the supported metric: \"{}\", cell scope has to be supported.",
                   metric_name.c_str());
    }
  }
  return true;
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
  logger.debug("Received RLC metrics: ue={} {}.", metrics.ue_index, metrics.rb_id.get_drb_id());
  ue_aggr_rlc_metrics[metrics.ue_index].ue_index = metrics.ue_index;
  if (metrics.rb_id.get_drb_id() == drb_id_t::drb1) {
    // Reset aggregated RLC metrics when metrics for drb1 are received.
    ue_aggr_rlc_metrics[metrics.ue_index].rx      = metrics.rx;
    ue_aggr_rlc_metrics[metrics.ue_index].tx      = metrics.tx;
    ue_aggr_rlc_metrics[metrics.ue_index].counter = 1;
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
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_pdus_no_segmentation += metrics.tx.num_pdus_no_segmentation;
    ue_aggr_rlc_metrics[metrics.ue_index].tx.num_pdu_bytes_no_segmentation += metrics.tx.num_pdu_bytes_no_segmentation;
    switch (ue_aggr_rlc_metrics[metrics.ue_index].tx.mode) {
      case rlc_mode::um_bidir:
      case rlc_mode::um_unidir_dl:
        ue_aggr_rlc_metrics[metrics.ue_index].tx.mode_specific.um.num_pdus_with_segmentation +=
            metrics.tx.mode_specific.um.num_pdus_with_segmentation;
        ue_aggr_rlc_metrics[metrics.ue_index].tx.mode_specific.um.num_pdu_bytes_with_segmentation +=
            metrics.tx.mode_specific.um.num_pdu_bytes_with_segmentation;
        break;
      case rlc_mode::am:
        ue_aggr_rlc_metrics[metrics.ue_index].tx.mode_specific.am.num_pdus_with_segmentation +=
            metrics.tx.mode_specific.am.num_pdus_with_segmentation;
        ue_aggr_rlc_metrics[metrics.ue_index].tx.mode_specific.am.num_pdu_bytes_with_segmentation +=
            metrics.tx.mode_specific.am.num_pdu_bytes_with_segmentation;
        ue_aggr_rlc_metrics[metrics.ue_index].tx.mode_specific.am.num_retx_pdus +=
            metrics.tx.mode_specific.am.num_retx_pdus;
        ue_aggr_rlc_metrics[metrics.ue_index].tx.mode_specific.am.num_retx_pdu_bytes +=
            metrics.tx.mode_specific.am.num_retx_pdu_bytes;
        ue_aggr_rlc_metrics[metrics.ue_index].tx.mode_specific.am.num_ctrl_pdus +=
            metrics.tx.mode_specific.am.num_ctrl_pdus;
        ue_aggr_rlc_metrics[metrics.ue_index].tx.mode_specific.am.num_ctrl_pdu_bytes +=
            metrics.tx.mode_specific.am.num_ctrl_pdu_bytes;
        break;
      default:
        // nothing to do here
        break;
    }
    ue_aggr_rlc_metrics[metrics.ue_index].counter++;
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
    logger.debug("Currently only NO_LABEL metric supported.");
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
    logger.debug("Metric {} not supported.", meas_type.meas_name().to_string());
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

float e2sm_kpm_du_meas_provider_impl::bytes_to_kbits(float value)
{
  constexpr unsigned nof_bits_per_byte = 8;
  return (nof_bits_per_byte * value / 1e3);
}

bool e2sm_kpm_du_meas_provider_impl::get_drb_dl_mean_throughput(
    const asn1::e2sm_kpm::label_info_list_l          label_info_list,
    const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
    const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
    std::vector<asn1::e2sm_kpm::meas_record_item_c>& items)
{
  bool meas_collected = false;
  if ((label_info_list.size() > 1 or
       (label_info_list.size() == 1 and not label_info_list[0].meas_label.no_label_present))) {
    logger.debug("Metric: DRB.UEThpDl supports only NO_LABEL label.");
    return meas_collected;
  }
  unsigned                     seconds = 1;
  std::map<uint16_t, unsigned> ue_throughput;
  if (ue_aggr_rlc_metrics.size() == 0) {
    return meas_collected;
  }
  for (auto& ue : ue_aggr_rlc_metrics) {
    size_t num_pdu_bytes_with_segmentation;
    switch (ue.second.tx.mode) {
      case rlc_mode::um_bidir:
      case rlc_mode::um_unidir_dl:
        num_pdu_bytes_with_segmentation = ue.second.tx.mode_specific.um.num_pdu_bytes_with_segmentation;
        break;
      case rlc_mode::am:
        num_pdu_bytes_with_segmentation = ue.second.tx.mode_specific.am.num_pdu_bytes_with_segmentation;
        break;
      default:
        num_pdu_bytes_with_segmentation = 0;
    }
    ue_throughput[ue.first] =
        bytes_to_kbits((ue.second.tx.num_pdu_bytes_no_segmentation + num_pdu_bytes_with_segmentation) /
                       ue.second.counter) /
        seconds; // unit is kbps
  }
  if (ues.size() == 0) {
    meas_record_item_c meas_record_item;
    int                total_throughput = 0;
    for (auto& ue : ue_throughput) {
      total_throughput += ue.second;
    }
    meas_record_item.set_integer() = total_throughput / ue_throughput.size();
    items.push_back(meas_record_item);
    meas_collected = true;
  }

  for (auto& ue : ues) {
    meas_record_item_c  meas_record_item;
    gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(ue.gnb_du_ueid().gnb_cu_ue_f1_ap_id);
    uint32_t            ue_idx            = f1ap_ue_id_provider.get_ue_index(gnb_cu_ue_f1ap_id);
    if (ue_throughput.count(ue_idx) == 0) {
      meas_record_item.set_no_value();
      items.push_back(meas_record_item);
      meas_collected = true;
      continue;
    }
    meas_record_item.set_integer() = ue_throughput[ue_idx];
    items.push_back(meas_record_item);
    meas_collected = true;
  }
  return meas_collected;
}

bool e2sm_kpm_du_meas_provider_impl::get_drb_ul_mean_throughput(
    const asn1::e2sm_kpm::label_info_list_l          label_info_list,
    const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
    const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
    std::vector<asn1::e2sm_kpm::meas_record_item_c>& items)
{
  bool meas_collected = false;
  if ((label_info_list.size() > 1 or
       (label_info_list.size() == 1 and not label_info_list[0].meas_label.no_label_present))) {
    logger.debug("Metric: DRB.UEThpUl supports only NO_LABEL label.");
    return meas_collected;
  }
  unsigned                     seconds = 1;
  std::map<uint16_t, unsigned> ue_throughput;
  if (ue_aggr_rlc_metrics.size() == 0) {
    return meas_collected;
  }
  for (auto& ue : ue_aggr_rlc_metrics) {
    ue_throughput[ue.first] = bytes_to_kbits(ue.second.rx.num_pdu_bytes / ue.second.counter) / seconds; // unit is kbps
  }
  if (ues.size() == 0) {
    meas_record_item_c meas_record_item;
    int                total_throughput = 0;
    for (auto& ue : ue_throughput) {
      total_throughput += ue.second;
    }
    meas_record_item.set_integer() = total_throughput / ue_throughput.size();
    items.push_back(meas_record_item);
    meas_collected = true;
  }

  for (auto& ue : ues) {
    meas_record_item_c  meas_record_item;
    gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(ue.gnb_du_ueid().gnb_cu_ue_f1_ap_id);
    uint32_t            ue_idx            = f1ap_ue_id_provider.get_ue_index(gnb_cu_ue_f1ap_id);
    if (ue_throughput.count(ue_idx) == 0) {
      meas_record_item.set_no_value();
      items.push_back(meas_record_item);
      meas_collected = true;
      continue;
    }
    meas_record_item.set_integer() = ue_throughput[ue_idx];
    items.push_back(meas_record_item);
    meas_collected = true;
  }
  return meas_collected;
}

bool e2sm_kpm_du_meas_provider_impl::get_drb_ul_success_rate(
    const asn1::e2sm_kpm::label_info_list_l          label_info_list,
    const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
    const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
    std::vector<asn1::e2sm_kpm::meas_record_item_c>& items)
{
  bool meas_collected = false;

  if ((label_info_list.size() > 1 or
       (label_info_list.size() == 1 and not label_info_list[0].meas_label.no_label_present))) {
    logger.debug("Metric: DRB.PacketSuccessRateUlgNBUu supports only NO_LABEL label.");
    return meas_collected;
  }
  if (cell_global_id.has_value()) {
    logger.debug("Metric: DRB.PacketSuccessRateUlgNBUu currently does not support cell_global_id filter.");
  }
  if (ues.size() == 0) {
    // E2 level measurements.
    meas_record_item_c meas_record_item;
    float              success_rate    = 0;
    uint32_t           total_lost_pdus = 0;
    uint32_t           total_pdus      = 0;
    for (unsigned idx = 0; ue_aggr_rlc_metrics.size(); idx++) {
      rlc_metrics& rlc_metric = ue_aggr_rlc_metrics[idx];
      total_lost_pdus += rlc_metric.rx.num_lost_pdus;
      total_pdus += rlc_metric.rx.num_pdus;
    }
    if (total_pdus) {
      success_rate = 1.0 * (total_pdus - total_lost_pdus) / total_pdus;
    }
    uint32_t success_rate_int      = success_rate * 100;
    meas_record_item.set_integer() = success_rate_int;
    items.push_back(meas_record_item);
    meas_collected = true;
  }

  for (auto& ue : ues) {
    meas_record_item_c  meas_record_item;
    gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(ue.gnb_du_ueid().gnb_cu_ue_f1_ap_id);
    uint32_t            ue_idx            = f1ap_ue_id_provider.get_ue_index(gnb_cu_ue_f1ap_id);
    if (ue_aggr_rlc_metrics.count(ue_idx) == 0) {
      meas_record_item.set_no_value();
      items.push_back(meas_record_item);
      meas_collected = true;
      continue;
    }
    float success_rate = 0;
    if (ue_aggr_rlc_metrics[ue_idx].rx.num_pdus) {
      success_rate = 1.0 * (ue_aggr_rlc_metrics[ue_idx].rx.num_pdus - ue_aggr_rlc_metrics[ue_idx].rx.num_lost_pdus) /
                     ue_aggr_rlc_metrics[ue_idx].rx.num_pdus;
    }
    uint32_t success_rate_int      = success_rate * 100;
    meas_record_item.set_integer() = success_rate_int;
    items.push_back(meas_record_item);
    meas_collected = true;
  }
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
    return meas_collected;
  }

  if (cell_global_id.has_value()) {
    logger.debug("Metric: DRB.RlcPacketDropRateDl currently does not support cell_global_id filter.");
  }

  if (ues.size() == 0) {
    // E2 level measurements.
    meas_record_item_c meas_record_item;
    float              drop_rate          = 0;
    uint32_t           total_dropped_sdus = 0;
    uint32_t           total_tx_num_sdus  = 0;
    for (auto& rlc_metric : ue_aggr_rlc_metrics) {
      total_dropped_sdus += rlc_metric.second.tx.num_dropped_sdus + rlc_metric.second.tx.num_discarded_sdus;
      total_tx_num_sdus += rlc_metric.second.tx.num_sdus;
    }
    if (total_tx_num_sdus) {
      drop_rate = 1.0 * total_dropped_sdus / total_tx_num_sdus;
    }
    uint32_t drop_rate_int         = drop_rate * 100;
    meas_record_item.set_integer() = drop_rate_int;
    items.push_back(meas_record_item);
    meas_collected = true;
  } else {
    // UE level measurements.
    for (auto& ue : ues) {
      meas_record_item_c  meas_record_item;
      gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(ue.gnb_du_ueid().gnb_cu_ue_f1_ap_id);
      uint32_t            ue_idx            = f1ap_ue_id_provider.get_ue_index(gnb_cu_ue_f1ap_id);
      if (ue_aggr_rlc_metrics.count(ue_idx) == 0) {
        meas_record_item.set_no_value();
        items.push_back(meas_record_item);
        meas_collected = true;
        continue;
      }
      float drop_rate = 0;
      if (ue_aggr_rlc_metrics[ue_idx].tx.num_sdus) {
        uint32_t dropped_sdus =
            ue_aggr_rlc_metrics[ue_idx].tx.num_dropped_sdus + ue_aggr_rlc_metrics[ue_idx].tx.num_discarded_sdus;
        drop_rate = 1.0 * dropped_sdus / ue_aggr_rlc_metrics[ue_idx].tx.num_sdus;
      }
      uint32_t drop_rate_int         = drop_rate * 100;
      meas_record_item.set_integer() = drop_rate_int;
      items.push_back(meas_record_item);
      meas_collected = true;
    }
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
    return false;
  }

  if (cell_global_id.has_value()) {
    logger.debug("Metric: DRB.RlcSduTransmittedVolumeDL currently does not support cell_global_id filter.");
  }

  if (ues.size() == 0) {
    // E2 level measurements.
    meas_record_item_c meas_record_item;
    size_t             total_tx_num_sdu_bytes = 0;
    for (auto& rlc_metric : ue_aggr_rlc_metrics) {
      total_tx_num_sdu_bytes += rlc_metric.second.tx.num_sdu_bytes;
    }
    meas_record_item.set_integer() = total_tx_num_sdu_bytes * 8 / 1000; // unit is kbit
    items.push_back(meas_record_item);
    meas_collected = true;
  } else {
    // UE level measurements.
    for (auto& ue : ues) {
      meas_record_item_c  meas_record_item;
      gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(ue.gnb_du_ueid().gnb_cu_ue_f1_ap_id);
      uint32_t            ue_idx            = f1ap_ue_id_provider.get_ue_index(gnb_cu_ue_f1ap_id);
      if (ue_aggr_rlc_metrics.count(ue_idx) == 0) {
        meas_record_item.set_no_value();
        items.push_back(meas_record_item);
        meas_collected = true;
        continue;
      }
      meas_record_item.set_integer() = ue_aggr_rlc_metrics[ue_idx].tx.num_sdu_bytes * 8 / 1000; // unit is kbit
      items.push_back(meas_record_item);
      meas_collected = true;
    }
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
    return meas_collected;
  }

  if (cell_global_id.has_value()) {
    logger.debug("Metric: DRB.RlcSduTransmittedVolumeUL currently does not support cell_global_id filter.");
  }

  if (ues.size() == 0) {
    // E2 level measurements.
    meas_record_item_c meas_record_item;
    size_t             total_rx_num_sdu_bytes = 0;
    for (auto& rlc_metric : ue_aggr_rlc_metrics) {
      total_rx_num_sdu_bytes += rlc_metric.second.rx.num_sdu_bytes;
    }
    meas_record_item.set_integer() = total_rx_num_sdu_bytes * 8 / 1000; // unit is kbit
    items.push_back(meas_record_item);
    meas_collected = true;
  } else {
    // UE level measurements.
    for (auto& ue : ues) {
      meas_record_item_c  meas_record_item;
      gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(ue.gnb_du_ueid().gnb_cu_ue_f1_ap_id);
      uint32_t            ue_idx            = f1ap_ue_id_provider.get_ue_index(gnb_cu_ue_f1ap_id);
      if (ue_aggr_rlc_metrics.count(ue_idx) == 0) {
        meas_record_item.set_no_value();
        items.push_back(meas_record_item);
        meas_collected = true;
        continue;
      }
      meas_record_item.set_integer() = ue_aggr_rlc_metrics[ue_idx].rx.num_sdu_bytes * 8 / 1000; // unit is kbit
      items.push_back(meas_record_item);
      meas_collected = true;
    }
  }
  return meas_collected;
}
