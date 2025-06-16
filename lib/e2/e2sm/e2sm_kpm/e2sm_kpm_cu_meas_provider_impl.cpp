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

#include "e2sm_kpm_cu_meas_provider_impl.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm;
using namespace srsran;

e2sm_kpm_cu_meas_provider_impl::e2sm_kpm_cu_meas_provider_impl() : logger(srslog::fetch_basic_logger("E2SM-KPM")) {}

e2sm_kpm_cu_cp_meas_provider_impl::e2sm_kpm_cu_cp_meas_provider_impl() : e2sm_kpm_cu_meas_provider_impl() {}

bool e2sm_kpm_cu_meas_provider_impl::check_e2sm_kpm_metrics_definitions(span<const e2sm_kpm_metric_t> metric_defs)
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

std::vector<std::string> e2sm_kpm_cu_meas_provider_impl::get_supported_metric_names(e2sm_kpm_metric_level_enum level)
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

bool e2sm_kpm_cu_meas_provider_impl::is_cell_supported(const asn1::e2sm::cgi_c& cell_global_id)
{
  // TODO: check if CELL is supported
  return true;
}

bool e2sm_kpm_cu_meas_provider_impl::is_ue_supported(const asn1::e2sm::ue_id_c& ueid)
{
  // TODO: check if UE is supported
  return true;
}

bool e2sm_kpm_cu_meas_provider_impl::is_test_cond_supported(const asn1::e2sm::test_cond_type_c& test_cond_type)
{
  // TODO: check if test condition is supported
  return true;
}

bool e2sm_kpm_cu_meas_provider_impl::is_metric_supported(const asn1::e2sm::meas_type_c&   meas_type,
                                                         const asn1::e2sm::meas_label_s&  label,
                                                         const e2sm_kpm_metric_level_enum level,
                                                         const bool&                      cell_scope)
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

bool e2sm_kpm_cu_meas_provider_impl::get_ues_matching_test_conditions(
    const asn1::e2sm::matching_cond_list_l& matching_cond_list,
    std::vector<asn1::e2sm::ue_id_c>&       ues)
{
  return true;
}

bool e2sm_kpm_cu_meas_provider_impl::get_ues_matching_test_conditions(
    const asn1::e2sm::matching_ue_cond_per_sub_list_l& matching_ue_cond_list,
    std::vector<asn1::e2sm::ue_id_c>&                  ues)
{
  return true;
}

bool e2sm_kpm_cu_meas_provider_impl::get_meas_data(const asn1::e2sm::meas_type_c&               meas_type,
                                                   const asn1::e2sm::label_info_list_l          label_info_list,
                                                   const std::vector<asn1::e2sm::ue_id_c>&      ues,
                                                   const std::optional<asn1::e2sm::cgi_c>       cell_global_id,
                                                   std::vector<asn1::e2sm::meas_record_item_c>& items)
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

bool e2sm_kpm_cu_meas_provider_impl::handle_no_meas_data_available(
    const std::vector<asn1::e2sm::ue_id_c>&        ues,
    std::vector<asn1::e2sm::meas_record_item_c>&   items,
    asn1::e2sm::meas_record_item_c::types::options value_type)
{
  if (ues.empty()) {
    // Fill with zero if E2 Node Measurement (Report Style 1)
    meas_record_item_c meas_record_item;
    if (value_type == asn1::e2sm::meas_record_item_c::types::options::integer) {
      meas_record_item.set_integer() = 0;
    } else if (value_type == asn1::e2sm::meas_record_item_c::types::options::real) {
      meas_record_item.set_real();
      meas_record_item.real().value = 0;
    } else if (value_type == asn1::e2sm::meas_record_item_c::types::options::not_satisfied) {
      meas_record_item.set_not_satisfied();
    } else {
      meas_record_item.set_no_value();
    }
    items.push_back(meas_record_item);
    return true;
  }
  return false;
}

float e2sm_kpm_cu_meas_provider_impl::bytes_to_kbits(float value)
{
  constexpr unsigned nof_bits_per_byte = 8;
  return (nof_bits_per_byte * value / 1e3);
}

void e2sm_kpm_cu_meas_provider_impl::report_metrics(const pdcp_metrics_container& metrics)
{
  ue_aggr_pdcp_metrics[metrics.ue_index].push_back(metrics);
  if (ue_aggr_pdcp_metrics[metrics.ue_index].size() > max_pdcp_metrics) {
    ue_aggr_pdcp_metrics[metrics.ue_index].pop_front();
  }
}

e2sm_kpm_cu_up_meas_provider_impl::e2sm_kpm_cu_up_meas_provider_impl() : e2sm_kpm_cu_meas_provider_impl()
{
  supported_metrics.emplace(
      "DRB.PdcpReordDelayUl",
      e2sm_kpm_supported_metric_t{
          NO_LABEL, E2_NODE_LEVEL, false, &e2sm_kpm_cu_up_meas_provider_impl::get_pdcp_reordering_delay_ul});
  supported_metrics.emplace(
      "DRB.PacketSuccessRateUlgNBUu",
      e2sm_kpm_supported_metric_t{
          NO_LABEL, E2_NODE_LEVEL, false, &e2sm_kpm_cu_up_meas_provider_impl::get_packet_success_rate_ul_gnb_uu});
}

bool e2sm_kpm_cu_meas_provider_impl::get_pdcp_reordering_delay_ul(const asn1::e2sm::label_info_list_l label_info_list,
                                                                  const std::vector<asn1::e2sm::ue_id_c>& ues,
                                                                  const std::optional<asn1::e2sm::cgi_c> cell_global_id,
                                                                  std::vector<asn1::e2sm::meas_record_item_c>& items)
{
  bool meas_collected = false;
  if (ue_aggr_pdcp_metrics.empty()) {
    return handle_no_meas_data_available(ues, items, asn1::e2sm::meas_record_item_c::types::options::real);
  }
  if ((label_info_list.size() > 1 or
       (label_info_list.size() == 1 and not label_info_list[0].meas_label.no_label_present))) {
    logger.debug("Metric: DRB.PDCP supports only NO_LABEL label.");
    return meas_collected;
  }
  if (ues.empty()) {
    // E2 level measurements.
    meas_record_item_c meas_record_item;
    float              av_ue_reordering_delay_us = 0;
    for (auto& pdcp_metric : ue_aggr_pdcp_metrics) {
      int num_reordering_records = std::accumulate(
          pdcp_metric.second.begin(),
          pdcp_metric.second.end(),
          0,
          [](size_t sum, const pdcp_metrics_container& metric) { return sum + metric.rx.reordering_counter; });
      int tot_reordering_delay_us = std::accumulate(
          pdcp_metric.second.begin(),
          pdcp_metric.second.end(),
          0,
          [](size_t sum, const pdcp_metrics_container& metric) { return sum + metric.rx.reordering_delay_us; });
      if (num_reordering_records && tot_reordering_delay_us) {
        av_ue_reordering_delay_us += (float)tot_reordering_delay_us / (float)num_reordering_records;
      }
    }
    if (av_ue_reordering_delay_us) {
      meas_record_item.set_real();
      meas_record_item.real().value = (av_ue_reordering_delay_us / ue_aggr_pdcp_metrics.size()) / 100; // unit is 0.1ms
      items.push_back(meas_record_item);
      meas_collected = true;
    } else {
      logger.warning("Invalid PDCP reordering delay value.");
      return meas_collected;
    }
  }
  return meas_collected;
}

bool e2sm_kpm_cu_meas_provider_impl::get_packet_success_rate_ul_gnb_uu(const label_info_list_l          label_info_list,
                                                                       const std::vector<ue_id_c>&      ues,
                                                                       const std::optional<cgi_c>       cell_global_id,
                                                                       std::vector<meas_record_item_c>& items)
{
  bool meas_collected = false;
  if (ue_aggr_pdcp_metrics.empty()) {
    return handle_no_meas_data_available(ues, items, meas_record_item_c::types::options::no_value);
  }
  if ((label_info_list.size() > 1 or
       (label_info_list.size() == 1 and not label_info_list[0].meas_label.no_label_present))) {
    logger.debug("Metric: DRB.PacketSuccessRateUlgNBUu supports only NO_LABEL label.");
    return meas_collected;
  }
  if (cell_global_id.has_value()) {
    logger.debug("Metric: DRB.PacketSuccessRateUlgNBUu currently does not support cell_global_id filter.");
  }
  if (ues.empty()) {
    // E2 level measurements.
    meas_record_item_c meas_record_item;
    float              success_rate = 0;
    uint32_t           total_sdus   = 0;
    uint32_t           total_pdus   = 0;
    for (auto& ue_metric : ue_aggr_pdcp_metrics) {
      total_sdus += std::accumulate(
          ue_metric.second.begin(), ue_metric.second.end(), 0, [](size_t sum, const pdcp_metrics_container& metric) {
            return sum + metric.rx.num_sdus;
          });
      total_pdus += std::accumulate(
          ue_metric.second.begin(), ue_metric.second.end(), 0, [](size_t sum, const pdcp_metrics_container& metric) {
            return sum + metric.rx.num_data_pdus;
          });
    }
    if (total_pdus) {
      success_rate = 1.0 * (total_sdus / total_pdus);
    }
    uint32_t success_rate_int      = success_rate * 100;
    meas_record_item.set_integer() = success_rate_int;
    items.push_back(meas_record_item);
    meas_collected = true;
  }

  return meas_collected;
}
