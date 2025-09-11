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

#include "cu_cp_unit_config_yaml_writer.h"
#include "apps/helpers/metrics/metrics_config_yaml_writer.h"
#include "cu_cp_unit_config.h"
#include "srsran/adt/span.h"

using namespace srsran;

static YAML::Node build_cu_cp_tai_slice_section(const cu_cp_unit_plmn_item::tai_slice_t& config)
{
  YAML::Node node;

  node["sst"] = config.sst;
  if (config.sd) {
    node["sd"] = config.sd;
  }

  return node;
}

static YAML::Node build_cu_cp_plmn_list_section(const cu_cp_unit_plmn_item& config)
{
  YAML::Node node;

  node["plmn"]        = config.plmn_id;
  auto tai_slice_node = node["tai_slice_support_list"];
  for (const auto& slice : config.tai_slice_support_list) {
    tai_slice_node.push_back(build_cu_cp_tai_slice_section(slice));
  }

  return node;
}

static YAML::Node build_cu_cp_supported_tas_section(const cu_cp_unit_supported_ta_item& config)
{
  YAML::Node node;

  node["tac"]    = config.tac;
  auto plmn_node = node["plmn_list"];
  for (const auto& plmn_item : config.plmn_list) {
    plmn_node.push_back(build_cu_cp_plmn_list_section(plmn_item));
  }

  return node;
}

static YAML::Node build_cu_cp_extra_amfs_item_section(const cu_cp_unit_amf_config_item& config)
{
  YAML::Node node;

  node["addr"]                   = config.ip_addr;
  node["port"]                   = config.port;
  node["bind_addr"]              = config.bind_addr;
  node["bind_interface"]         = config.bind_interface;
  node["sctp_rto_initial"]       = config.sctp_rto_initial_ms;
  node["sctp_rto_min"]           = config.sctp_rto_min_ms;
  node["sctp_rto_max"]           = config.sctp_rto_max_ms;
  node["sctp_init_max_attempts"] = config.sctp_init_max_attempts;
  node["sctp_max_init_timeo"]    = config.sctp_max_init_timeo_ms;
  node["sctp_hb_interval"]       = config.sctp_hb_interval_ms;
  node["sctp_assoc_max_retx"]    = config.sctp_assoc_max_retx;
  node["sctp_nodelay"]           = config.sctp_nodelay;

  auto sta_node = node["supported_tracking_areas"];
  for (const auto& ta : config.supported_tas) {
    sta_node.push_back(build_cu_cp_supported_tas_section(ta));
  }

  return node;
}

static YAML::Node build_cu_cp_extra_amfs_section(const std::vector<cu_cp_unit_amf_config_item>& amfs)
{
  YAML::Node node;

  for (const auto& amf : amfs) {
    node.push_back(build_cu_cp_extra_amfs_item_section(amf));
  }

  return node;
}

static YAML::Node build_cu_cp_amf_section(const cu_cp_unit_amf_config& config)
{
  YAML::Node node;

  node["no_core"]                     = config.no_core;
  node["amf_reconnection_retry_time"] = config.amf_reconnection_retry_time;
  node["amf"]                         = build_cu_cp_extra_amfs_item_section(config.amf);

  return node;
}

static YAML::Node build_cu_cp_mobility_ncells_section(const cu_cp_unit_neighbor_cell_config_item& config)
{
  YAML::Node node;

  node["nr_cell_id"] = config.nr_cell_id;
  for (auto report_id : config.report_cfg_ids) {
    node["report_configs"] = report_id;
  }

  node["report_configs"].SetStyle(YAML::EmitterStyle::Flow);

  return node;
}

static YAML::Node build_cu_cp_mobility_cells_section(const cu_cp_unit_cell_config_item& config)
{
  YAML::Node node;

  node["nr_cell_id"] = config.nr_cell_id;
  if (config.periodic_report_cfg_id) {
    node["periodic_report_cfg_id"] = config.periodic_report_cfg_id.value();
  }
  if (config.band) {
    node["band"] = static_cast<unsigned>(config.band.value());
  }
  node["gnb_id_bit_length"] = config.gnb_id_bit_length.value();
  if (config.pci) {
    node["pci"] = config.pci.value();
  }
  if (config.ssb_arfcn) {
    node["ssb_arfcn"] = config.ssb_arfcn.value();
  }
  if (config.ssb_scs) {
    node["ssb_scs"] = config.ssb_scs.value();
  }
  if (config.ssb_period) {
    node["ssb_period"] = config.ssb_period.value();
  }
  if (config.ssb_offset) {
    node["ssb_offset"] = config.ssb_offset.value();
  }
  if (config.ssb_duration) {
    node["ssb_duration"] = config.ssb_duration.value();
  }

  for (const auto& ncell : config.ncells) {
    node["ncells"] = build_cu_cp_mobility_ncells_section(ncell);
  }

  return node;
}

static YAML::Node build_cu_cp_mobility_report_section(const cu_cp_unit_report_config& config)
{
  YAML::Node node;

  node["report_cfg_id"] = config.report_cfg_id;
  node["report_type"]   = config.report_type;
  if (config.event_triggered_report_type) {
    node["event_triggered_report_type"] = config.event_triggered_report_type.value();
    if (config.meas_trigger_quantity_threshold_db) {
      node["meas_trigger_quantity_threshold_db"] = config.meas_trigger_quantity_threshold_db.value();
    }
    if (config.meas_trigger_quantity_threshold_2_db) {
      node["meas_trigger_quantity_threshold_2_db"] = config.meas_trigger_quantity_threshold_2_db.value();
    }
    if (config.meas_trigger_quantity_offset_db) {
      node["meas_trigger_quantity_offset_db"] = config.meas_trigger_quantity_offset_db.value();
    }
    if (config.hysteresis_db) {
      node["hysteresis_db"] = config.hysteresis_db.value();
    }
    if (config.meas_trigger_quantity) {
      node["meas_trigger_quantity"] = config.meas_trigger_quantity.value();
    }
    if (config.time_to_trigger_ms) {
      node["time_to_trigger_ms"] = config.time_to_trigger_ms.value();
    }
  }

  node["report_interval_ms"] = config.report_interval_ms;

  return node;
}

static YAML::Node build_cu_cp_mobility_section(const cu_cp_unit_mobility_config& config)
{
  YAML::Node node;

  node["trigger_handover_from_measurements"] = config.trigger_handover_from_measurements;
  for (const auto& cell : config.cells) {
    node["cells"] = build_cu_cp_mobility_cells_section(cell);
  }
  for (const auto& report : config.report_configs) {
    node["report_configs"] = build_cu_cp_mobility_report_section(report);
  }

  return node;
}

static YAML::Node build_cu_cp_rrc_section(const cu_cp_unit_rrc_config& config)
{
  YAML::Node node;

  node["force_reestablishment_fallback"] = config.force_reestablishment_fallback;
  node["rrc_procedure_guard_time_ms"]    = config.rrc_procedure_guard_time_ms;

  return node;
}

static YAML::Node build_cu_cp_security_section(const cu_cp_unit_security_config& config)
{
  YAML::Node node;

  node["integrity"]       = config.integrity_protection;
  node["confidentiality"] = config.confidentiality_protection;
  node["nea_pref_list"]   = config.nea_preference_list;
  node["nia_pref_list"]   = config.nia_preference_list;

  return node;
}

static YAML::Node build_cu_cp_f1ap_section(const cu_cp_unit_f1ap_config& config)
{
  YAML::Node node;

  node["procedure_timeout"] = config.procedure_timeout;

  return node;
}

static YAML::Node build_cu_cp_e1ap_section(const cu_cp_unit_e1ap_config& config)
{
  YAML::Node node;

  node["procedure_timeout"] = config.procedure_timeout;

  return node;
}

static YAML::Node build_cu_cp_section(const cu_cp_unit_config& config)
{
  YAML::Node node;

  node["max_nof_dus"]                 = config.max_nof_dus;
  node["max_nof_cu_ups"]              = config.max_nof_cu_ups;
  node["max_nof_ues"]                 = config.max_nof_ues;
  node["max_nof_drbs_per_ue"]         = static_cast<unsigned>(config.max_nof_drbs_per_ue);
  node["inactivity_timer"]            = config.inactivity_timer;
  node["request_pdu_session_timeout"] = config.request_pdu_session_timeout;

  node["amf"] = build_cu_cp_amf_section(config.amf_config);
  if (!config.extra_amfs.empty()) {
    node["extra_amfs"] = build_cu_cp_extra_amfs_section(config.extra_amfs);
  }
  node["mobility"] = build_cu_cp_mobility_section(config.mobility_config);
  node["rrc"]      = build_cu_cp_rrc_section(config.rrc_config);
  node["security"] = build_cu_cp_security_section(config.security_config);
  node["f1ap"]     = build_cu_cp_f1ap_section(config.f1ap_config);
  node["e1ap"]     = build_cu_cp_e1ap_section(config.e1ap_config);

  return node;
}

static void fill_cu_cp_log_section(YAML::Node node, const cu_cp_unit_logger_config& config)
{
  node["pdcp_level"]        = srslog::basic_level_to_string(config.pdcp_level);
  node["rrc_level"]         = srslog::basic_level_to_string(config.rrc_level);
  node["ngap_level"]        = srslog::basic_level_to_string(config.ngap_level);
  node["nrppa_level"]       = srslog::basic_level_to_string(config.nrppa_level);
  node["e1ap_level"]        = srslog::basic_level_to_string(config.e1ap_level);
  node["f1ap_level"]        = srslog::basic_level_to_string(config.f1ap_level);
  node["cu_level"]          = srslog::basic_level_to_string(config.cu_level);
  node["sec_level"]         = srslog::basic_level_to_string(config.sec_level);
  node["hex_max_size"]      = config.hex_max_size;
  node["e1ap_json_enabled"] = config.e1ap_json_enabled;
  node["f1ap_json_enabled"] = config.f1ap_json_enabled;
}

static void fill_cu_cp_pcap_section(YAML::Node node, const cu_cp_unit_pcap_config& config)
{
  node["ngap_filename"] = config.ngap.filename;
  node["ngap_enable"]   = config.ngap.enabled;
  node["f1ap_filename"] = config.f1ap.filename;
  node["f1ap_enable"]   = config.f1ap.enabled;
  node["e1ap_filename"] = config.e1ap.filename;
  node["e1ap_enable"]   = config.e1ap.enabled;
}

static void fill_cu_cp_metrics_layers_section(YAML::Node node, const cu_cp_unit_metrics_layer_config& config)
{
  node["enable_ngap"] = config.enable_ngap;
  node["enable_pdcp"] = config.enable_pdcp;
  node["enable_rrc"]  = config.enable_rrc;
}

static void fill_cu_cp_metrics_section(YAML::Node node, const cu_cp_unit_metrics_config& config)
{
  auto perdiodicity_node                   = node["periodicity"];
  perdiodicity_node["cu_cp_report_period"] = config.cu_cp_report_period;

  fill_cu_cp_metrics_layers_section(node["layers"], config.layers_cfg);
}

static void fill_cu_cp_am_section(YAML::Node node, const cu_cp_unit_rlc_am_config& config)
{
  {
    YAML::Node tx_node            = node["tx"];
    tx_node["sn"]                 = config.tx.sn_field_length;
    tx_node["t-poll-retransmit"]  = config.tx.t_poll_retx;
    tx_node["max-retx-threshold"] = config.tx.max_retx_thresh;
    tx_node["poll-pdu"]           = config.tx.poll_pdu;
    tx_node["poll-byte"]          = config.tx.poll_byte;
    tx_node["max_window"]         = config.tx.max_window;
    tx_node["queue-size"]         = config.tx.queue_size;
  }
  {
    YAML::Node rx_node           = node["rx"];
    rx_node["sn"]                = config.rx.sn_field_length;
    rx_node["t-reassembly"]      = config.rx.t_reassembly;
    rx_node["t-status-prohibit"] = config.rx.t_status_prohibit;
    rx_node["max_sn_per_status"] = config.rx.max_sn_per_status;
  }
}

static void fill_cu_cp_um_bidir_section(YAML::Node node, const cu_cp_unit_rlc_um_config& config)
{
  {
    YAML::Node tx_node    = node["tx"];
    tx_node["sn"]         = config.tx.sn_field_length;
    tx_node["queue-size"] = config.tx.queue_size;
  }
  {
    YAML::Node rx_node      = node["rx"];
    rx_node["sn"]           = config.rx.sn_field_length;
    rx_node["t-reassembly"] = config.rx.t_reassembly;
  }
}

static void fill_cu_cp_rlc_qos_section(YAML::Node node, const cu_cp_unit_rlc_config& config)
{
  node["mode"] = config.mode;
  if (config.mode == "am") {
    fill_cu_cp_am_section(node["am"], config.am);
  }

  if (config.mode == "um-bidir") {
    fill_cu_cp_um_bidir_section(node["um-bidir"], config.um);
  }
}

static void fill_cu_cp_pdcp_qos_section(YAML::Node node, const cu_cp_unit_pdcp_config& config)
{
  {
    YAML::Node tx_node                = node["tx"];
    tx_node["sn"]                     = config.tx.sn_field_length;
    tx_node["discard_timer"]          = config.tx.discard_timer;
    tx_node["status_report_required"] = config.tx.status_report_required;
  }
  {
    YAML::Node rx_node               = node["rx"];
    rx_node["sn"]                    = config.rx.sn_field_length;
    rx_node["t_reordering"]          = config.rx.t_reordering;
    rx_node["out_of_order_delivery"] = config.rx.out_of_order_delivery;
  }
}

static void fill_cu_cp_qos_entry(YAML::Node node, const cu_cp_unit_qos_config& config)
{
  node["five_qi"] = five_qi_to_uint(config.five_qi);
  fill_cu_cp_rlc_qos_section(node["rlc"], config.rlc);
  fill_cu_cp_pdcp_qos_section(node["pdcp"], config.pdcp);
}

static YAML::Node get_last_entry(YAML::Node node)
{
  srsran_assert(node.size() > 0, "Node is empty");

  auto it = node.begin();
  for (unsigned i = 1; i != node.size(); ++i) {
    ++it;
  }
  return *it;
}

static void fill_cu_cp_qos_section(YAML::Node node, span<const cu_cp_unit_qos_config> qos_cfg)
{
  auto qos_node = node["qos"];
  for (const auto& qos : qos_cfg) {
    auto node_entry = std::find_if(qos_node.begin(), qos_node.end(), [five = qos.five_qi](const YAML::Node& tmp) {
      return static_cast<uint16_t>(five) == tmp["five_qi"].as<uint16_t>();
    });
    if (node_entry != qos_node.end()) {
      YAML::Node node_five = *node_entry;
      fill_cu_cp_qos_entry(node_five, qos);
    } else {
      qos_node.push_back(YAML::Node());
      fill_cu_cp_qos_entry(get_last_entry(qos_node), qos);
    }
  }
}

static void build_cu_cp_slicing_section(YAML::Node node, span<const s_nssai_t> slice_cfg)
{
  for (const auto& slice : slice_cfg) {
    YAML::Node node_entry;
    node_entry["sst"] = slice.sst.value();
    if (slice.sd.is_set()) {
      node_entry["sd"] = slice.sd.value();
    }
    node["slicing"].push_back(node_entry);
  }
}

void srsran::fill_cu_cp_config_in_yaml_schema(YAML::Node& node, const cu_cp_unit_config& config)
{
  node["gnb_id"]            = config.gnb_id.id;
  node["gnb_id_bit_length"] = static_cast<unsigned>(config.gnb_id.bit_length);
  node["ran_node_name"]     = config.ran_node_name;

  app_helpers::fill_metrics_appconfig_in_yaml_schema(node, config.metrics.common_metrics_cfg);

  node["cu_cp"] = build_cu_cp_section(config);
  fill_cu_cp_log_section(node["log"], config.loggers);
  fill_cu_cp_pcap_section(node["pcap"], config.pcap_cfg);
  fill_cu_cp_metrics_section(node["metrics"], config.metrics);
  fill_cu_cp_qos_section(node, config.qos_cfg);

  build_cu_cp_slicing_section(node, config.slice_cfg);
}
