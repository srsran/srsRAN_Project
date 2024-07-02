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

#include "cu_cp_unit_config_validator.h"
#include "srsran/adt/span.h"
#include "srsran/pdcp/pdcp_t_reordering.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/rlc/rlc_config.h"
#include <map>
#include <set>
#include <sstream>

using namespace srsran;

static bool validate_mobility_appconfig(gnb_id_t gnb_id, const cu_cp_unit_mobility_config& config)
{
  // check that report config ids are unique
  std::map<unsigned, std::string> report_cfg_ids_to_report_type;
  for (const auto& report_cfg : config.report_configs) {
    if (report_cfg_ids_to_report_type.find(report_cfg.report_cfg_id) != report_cfg_ids_to_report_type.end()) {
      fmt::print("Report config ids must be unique\n");
      return false;
    }
    report_cfg_ids_to_report_type.emplace(report_cfg.report_cfg_id, report_cfg.report_type);
  }

  std::map<nr_cell_identity, std::set<unsigned>> cell_to_report_cfg_id;

  // check cu_cp_cell_config
  std::set<nr_cell_identity> ncis;
  for (const auto& cell : config.cells) {
    nr_cell_identity nci = nr_cell_identity::create(cell.nr_cell_id).value();
    if (!ncis.emplace(nci).second) {
      fmt::print("Cells must be unique ({:#x} already present)\n", cell.nr_cell_id);
      return false;
    }

    if (cell.ssb_period.has_value() && cell.ssb_offset.has_value() &&
        cell.ssb_offset.value() >= cell.ssb_period.value()) {
      fmt::print("ssb_offset must be smaller than ssb_period\n");
      return false;
    }

    if (cell.periodic_report_cfg_id.has_value()) {
      // try to add report config id to cell_to_report_cfg_id map
      cell_to_report_cfg_id.emplace(nci, std::set<unsigned>());
      auto& report_cfg_ids = cell_to_report_cfg_id.at(nci);
      if (!report_cfg_ids.emplace(cell.periodic_report_cfg_id.value()).second) {
        fmt::print("cell={}: report_config_id={} already configured for this cell)\n",
                   cell.nr_cell_id,
                   cell.periodic_report_cfg_id.value());
        return false;
      }
      // check that for the serving cell only periodic reports are configured
      if (report_cfg_ids_to_report_type.at(cell.periodic_report_cfg_id.value()) != "periodical") {
        fmt::print("For the serving cell only periodic reports are allowed\n");
        return false;
      }
    }

    // check if cell is an external managed cell
    if (nci.gnb_id(gnb_id.bit_length) != gnb_id) {
      if (!cell.gnb_id_bit_length.has_value() || !cell.pci.has_value() || !cell.band.has_value() ||
          !cell.ssb_arfcn.has_value() || !cell.ssb_scs.has_value() || !cell.ssb_period.has_value() ||
          !cell.ssb_offset.has_value() || !cell.ssb_duration.has_value()) {
        fmt::print("cell={:#x}: For external cells, the gnb_id_bit_length, pci, band, ssb_arfcn, ssb_scs, ssb_period, "
                   "ssb_offset and "
                   "ssb_duration must be configured in the mobility config\n",
                   cell.nr_cell_id);
        return false;
      }
    } else {
      if (cell.pci.has_value() || cell.band.has_value() || cell.ssb_arfcn.has_value() || cell.ssb_scs.has_value() ||
          cell.ssb_period.has_value() || cell.ssb_offset.has_value() || cell.ssb_duration.has_value()) {
        fmt::print("cell={:#x}: For cells managed by the CU-CP the gnb_id_bit_length, pci, band, ssb_arfcn, ssb_scs, "
                   "ssb_period, "
                   "ssb_offset and "
                   "ssb_duration must not be configured in the mobility config\n",
                   cell.nr_cell_id);
        return false;
      }
    }

    // check that for neighbor cells managed by this CU-CP no periodic reports are configured
    for (const auto& ncell : cell.ncells) {
      // try to add report config ids to cell_to_report_cfg_id map
      for (const auto& id : ncell.report_cfg_ids) {
        if (cell_to_report_cfg_id.find(nci) != cell_to_report_cfg_id.end() &&
            !cell_to_report_cfg_id.at(nci).emplace(id).second) {
          fmt::print("cell={}: report_config_id={} already configured for this cell\n", ncell.nr_cell_id, id);
          return false;
        }
      }
    }
  }

  // verify that each configured neighbor cell is present
  for (const auto& cell : config.cells) {
    nr_cell_identity nci = nr_cell_identity::create(cell.nr_cell_id).value();
    for (const auto& ncell : cell.ncells) {
      if (ncis.find(nci) == ncis.end()) {
        fmt::print("Neighbor cell config for nci={:#x} incomplete. No valid configuration for cell nci={:#x} found.\n",
                   cell.nr_cell_id,
                   ncell.nr_cell_id);
        return false;
      }
    }
  }

  return true;
}

/// Validates the given security configuration. Returns true on success, otherwise false.
static bool validate_security_appconfig(const cu_cp_unit_security_config& config)
{
  // String splitter helper
  auto split = [](const std::string& s, char delim) -> std::vector<std::string> {
    std::vector<std::string> result;
    std::stringstream        ss(s);
    std::string              item;

    while (getline(ss, item, delim)) {
      result.push_back(item);
    }

    return result;
  };

  // > Remove spaces, convert to lower case and split on comma
  std::string nea_preference_list = config.nea_preference_list;
  nea_preference_list.erase(std::remove_if(nea_preference_list.begin(), nea_preference_list.end(), ::isspace),
                            nea_preference_list.end());
  std::transform(nea_preference_list.begin(),
                 nea_preference_list.end(),
                 nea_preference_list.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  std::vector<std::string> nea_v = split(nea_preference_list, ',');

  // > Check valid ciphering algos
  for (const std::string& algo : nea_v) {
    if (algo != "nea0" and algo != "nea1" and algo != "nea2" and algo != "nea3") {
      fmt::print("Invalid ciphering algorithm. Valid values are \"nea0\", \"nia1\", \"nia2\" and \"nia3\". algo={}\n",
                 algo);
      return false;
    }
  }

  // > Remove spaces, convert to lower case and split on comma
  std::string nia_preference_list = config.nia_preference_list;
  nia_preference_list.erase(std::remove_if(nia_preference_list.begin(), nia_preference_list.end(), ::isspace),
                            nia_preference_list.end());
  std::transform(nia_preference_list.begin(),
                 nia_preference_list.end(),
                 nia_preference_list.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  std::vector<std::string> nia_v = split(nia_preference_list, ',');

  // > Check valid integrity algos
  for (const std::string& algo : nia_v) {
    if (algo == "nia0") {
      fmt::print("NIA0 cannot be selected in the algorithm preferences.\n");
      return false;
    }
    if (algo != "nia1" and algo != "nia2" and algo != "nia3") {
      fmt::print("Invalid integrity algorithm. Valid values are \"nia1\", \"nia2\" and \"nia3\". algo={}\n", algo);
      return false;
    }
  }

  return true;
}

/// Validates the given PDCP configuration. Returns true on success, otherwise false.
static bool validate_pdcp_appconfig(five_qi_t five_qi, const cu_cp_unit_pdcp_config& config)
{
  if (config.integrity_protection_required) {
    fmt::print("PDCP DRB integrity protection is not supported yet. {}\n", five_qi);
    return false;
  }

  // Check TX.
  if (config.tx.sn_field_length != 12 && config.tx.sn_field_length != 18) {
    fmt::print("PDCP TX SN length is neither 12 or 18 bits. {} SN={}\n", five_qi, config.tx.sn_field_length);
    return false;
  }
  if (config.tx.status_report_required) {
    fmt::print("PDCP TX status report required not supported yet. {}\n", five_qi);
    return false;
  }

  // Check RX.
  if (config.rx.sn_field_length != 12 && config.rx.sn_field_length != 18) {
    fmt::print("PDCP RX SN length is neither 12 or 18 bits. {} SN={}\n", five_qi, config.rx.sn_field_length);
    return false;
  }

  pdcp_t_reordering t_reordering = {};
  if (!pdcp_t_reordering_from_int(t_reordering, config.rx.t_reordering)) {
    fmt::print("PDCP RX t-Reordering is not a valid value. {}, t-Reordering={}\n", five_qi, config.rx.t_reordering);
    fmt::print("Valid values: "
               "\"infinity, ms0, ms1, ms2, ms4, ms5, ms8, ms10, ms15, ms20, ms30, ms40,ms50, ms60, ms80, "
               "ms100, ms120, ms140, ms160, ms180, ms200, ms220,ms240, ms260, ms280, ms300, ms500, ms750, ms1000, "
               "ms1250, ms1500, ms1750, ms2000, ms2250, ms2500, ms2750\"\n");
    return false;
  }
  if (t_reordering == pdcp_t_reordering::infinity) {
    fmt::print("PDCP t-Reordering=infinity on DRBs is not advised. It can cause data stalls. {}\n", five_qi);
  }

  if (config.rx.out_of_order_delivery) {
    fmt::print("PDCP RX out-of-order delivery is not supported. {}\n", five_qi);
    return false;
  }
  return true;
}

static bool validate_rlc_um_appconfig(five_qi_t five_qi, const cu_cp_unit_rlc_um_config& config)
{
  // Validate TX.

  rlc_um_sn_size tmp_sn_size;
  if (!from_number(tmp_sn_size, config.tx.sn_field_length)) {
    fmt::print("RLC UM TX SN length is neither 6 or 12 bits. {} sn_size={}\n", five_qi, config.tx.sn_field_length);
    return false;
  }

  if (config.tx.queue_size == 0) {
    fmt::print("RLC TX queue size cannot be 0. {}\n", five_qi);
    return false;
  }

  // Validate RX.

  if (!from_number(tmp_sn_size, config.rx.sn_field_length)) {
    fmt::print("RLC TX queue size cannot be 0. {}\n", five_qi);
    return false;
  }

  rlc_t_reassembly tmp_t_reassembly;
  if (!rlc_t_reassembly_from_int(tmp_t_reassembly, config.rx.t_reassembly)) {
    fmt::print("RLC UM RX t-Reassembly is invalid. {} t_reassembly={}\n", five_qi, config.rx.t_reassembly);
    fmt::print("Valid values are:"
               " ms40, ms45, ms50, ms55, ms60, ms65, ms70,"
               " ms75, ms80, ms85, ms90, ms95, ms100, ms110,"
               " ms120, ms130, ms140, ms150, ms160, ms170,"
               " ms180, ms190, ms200\n");
    return false;
  }
  return true;
}

template <typename id_type>
static bool validate_rlc_am_appconfig(id_type id, const cu_cp_unit_rlc_am_config& config)
{
  // Validate TX.

  rlc_am_sn_size tmp_sn_size;
  if (!from_number(tmp_sn_size, config.tx.sn_field_length)) {
    fmt::print("RLC AM TX SN length is neither 12 or 18 bits. {} sn_size={}\n", id, config.tx.sn_field_length);
    return false;
  }

  rlc_t_poll_retransmit tmp_t_poll_retransmit;
  if (!rlc_t_poll_retransmit_from_int(tmp_t_poll_retransmit, config.tx.t_poll_retx)) {
    fmt::print("Invalid RLC AM TX t-PollRetransmission. {} t_poll_retx={}\n", id, config.tx.t_poll_retx);
    fmt::print(" Valid values are: ms5, ms10, ms15, ms20, ms25, ms30, ms35,"
               " ms40, ms45, ms50, ms55, ms60, ms65, ms70, ms75, ms80, ms85,"
               " ms90, ms95, ms100, ms105, ms110, ms115, ms120, ms125, ms130,"
               " ms135, ms140, ms145, ms150, ms155, ms160, ms165, ms170, ms175,"
               " ms180, ms185, ms190, ms195, ms200, ms205, ms210, ms215, ms220,"
               " ms225, ms230, ms235, ms240, ms245, ms250, ms300, ms350, ms400,"
               " ms450, ms500, ms800, ms1000, ms2000, ms4000\n");
    return false;
  }

  rlc_max_retx_threshold tmp_max_retx_threshold;
  if (!rlc_max_retx_threshold_from_int(tmp_max_retx_threshold, config.tx.max_retx_thresh)) {
    fmt::print("Invalid RLC AM TX max retx threshold. {} max_retx_threshold={}\n", id, config.tx.max_retx_thresh);
    fmt::print(" Valid values are: t1, t2, t3, t4, t6, t8, t16, t32\n");
    return false;
  }

  rlc_poll_pdu tmp_poll_pdu;
  if (!rlc_poll_pdu_from_int(tmp_poll_pdu, config.tx.poll_pdu)) {
    fmt::print("Invalid RLC AM TX PollPDU. {} poll_pdu={}\n", id, config.tx.poll_pdu);
    fmt::print(" Valid values are:"
               "p4, p8, p16, p32, p64, p128, p256, p512, p1024, p2048,"
               " p4096, p6144, p8192, p12288, p16384,p20480,"
               " p24576, p28672, p32768, p40960, p49152, p57344, p65536\n");
    return false;
  }

  rlc_poll_kilo_bytes tmp_poll_bytes;
  if (!rlc_poll_kilo_bytes_from_int(tmp_poll_bytes, config.tx.poll_byte)) {
    fmt::print("Invalid RLC AM TX PollBytes. {} poll_bytes={}\n", id, config.tx.poll_byte);
    fmt::print(" Valid values are (in KBytes):"
               " kB1, kB2, kB5, kB8, kB10, kB15, kB25, kB50, kB75,"
               " kB100, kB125, kB250, kB375, kB500, kB750, kB1000,"
               " kB1250, kB1500, kB2000, kB3000, kB4000, kB4500,"
               " kB5000, kB5500, kB6000, kB6500, kB7000, kB7500,"
               " mB8, mB9, mB10, mB11, mB12, mB13, mB14, mB15,"
               " mB16, mB17, mB18, mB20, mB25, mB30, mB40, infinity\n");
    return false;
  }

  if (config.tx.queue_size == 0) {
    fmt::print("RLC AM TX queue size cannot be 0. {}\n", id);
    return false;
  }

  // Validate RX.

  if (!from_number(tmp_sn_size, config.rx.sn_field_length)) {
    fmt::print("RLC AM RX SN length is neither 12 or 18 bits. {} sn_size={}\n", id, config.rx.sn_field_length);
    return false;
  }

  rlc_t_reassembly tmp_t_reassembly;
  if (!rlc_t_reassembly_from_int(tmp_t_reassembly, config.rx.t_reassembly)) {
    fmt::print("RLC AM RX t-Reassembly is invalid. {} t_reassembly={}\n", id, config.rx.t_reassembly);
    fmt::print("Valid values are:"
               " ms40, ms45, ms50, ms55, ms60, ms65, ms70,"
               " ms75, ms80, ms85, ms90, ms95, ms100, ms110,"
               " ms120, ms130, ms140, ms150, ms160, ms170,"
               " ms180, ms190, ms200\n");
    return false;
  }

  rlc_t_status_prohibit tmp_t_status_prohibit;
  if (!rlc_t_status_prohibit_from_int(tmp_t_status_prohibit, config.rx.t_status_prohibit)) {
    fmt::print("RLC AM RX t-statusProhibit is invalid. {} t_status_prohibit={}\n", id, config.rx.t_status_prohibit);
    fmt::print("Valid values are:"
               "ms0, ms5, ms10, ms15, ms20, ms25, ms30, ms35,"
               "ms40, ms45, ms50, ms55, ms60, ms65, ms70,"
               "ms75, ms80, ms85, ms90, ms95, ms100, ms105,"
               "ms110, ms115, ms120, ms125, ms130, ms135,"
               "ms140, ms145, ms150, ms155, ms160, ms165,"
               "ms170, ms175, ms180, ms185, ms190, ms195,"
               "ms200, ms205, ms210, ms215, ms220, ms225,"
               "ms230, ms235, ms240, ms245, ms250, ms300,"
               "ms350, ms400, ms450, ms500, ms800, ms1000,"
               "ms1200, ms1600, ms2000, ms2400\n");
    return false;
  }

  if (config.rx.max_sn_per_status >= window_size(config.rx.sn_field_length)) {
    fmt::print("RLC AM RX max_sn_per_status={} exceeds window_size={}. sn_size={}\n",
               config.rx.max_sn_per_status,
               window_size(config.rx.sn_field_length),
               config.rx.sn_field_length);
    return false;
  }

  return true;
}

static bool validate_rlc_appconfig(five_qi_t five_qi, const cu_cp_unit_rlc_config& config)
{
  // Check mode.
  if (config.mode != "am" && config.mode != "um-bidir") {
    fmt::print("RLC mode is neither \"am\" or \"um-bidir\". {} mode={}\n", five_qi, config.mode);
    return false;
  }

  // Check AM.
  if (config.mode == "am" && !validate_rlc_am_appconfig(five_qi, config.am)) {
    fmt::print("RLC AM config is invalid. {}\n", five_qi);
    return false;
  }

  // Check UM.
  if (config.mode == "um-bidir" && !validate_rlc_um_appconfig(five_qi, config.um)) {
    fmt::print("RLC UM config is invalid. {}\n", five_qi);
    return false;
  }
  return true;
}

/// Validates the given QoS configuration. Returns true on success, otherwise false.
static bool validate_qos_appconfig(span<const cu_cp_unit_qos_config> config)
{
  for (const auto& qos : config) {
    if (!validate_pdcp_appconfig(qos.five_qi, qos.pdcp)) {
      return false;
    }
    if (!validate_rlc_appconfig(qos.five_qi, qos.rlc)) {
      return false;
    }
  }
  return true;
}

/// Validates the given AMF configuration. Returns true on success, otherwise false.
static bool validate_amf_appconfig(const cu_cp_unit_amf_config& config)
{
  // only check for non-empty AMF address and default port
  if (config.ip_addr.empty() or config.port != 38412) {
    return false;
  }
  return true;
}

/// Validates the given CU-CP configuration. Returns true on success, otherwise false.
static bool validate_cu_cp_appconfig(const gnb_id_t gnb_id, const cu_cp_unit_config& config)
{
  if (!validate_amf_appconfig(config.amf_cfg)) {
    return false;
  }

  // validate mobility config
  if (!validate_mobility_appconfig(gnb_id, config.mobility_config)) {
    return false;
  }

  if (!validate_security_appconfig(config.security_config)) {
    return false;
  }

  if (!validate_qos_appconfig(config.qos_cfg)) {
    return false;
  }

  if (config.plmns.size() != config.tacs.size()) {
    fmt::print("Number of PLMNs '{}' do not match the number of TACs '{}'\n", config.plmns.size(), config.tacs.size());

    return false;
  }

  return true;
}

bool srsran::validate_cu_cp_unit_config(const cu_cp_unit_config& config)
{
  return validate_cu_cp_appconfig(config.gnb_id, config);
}
