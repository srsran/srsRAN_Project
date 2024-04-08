/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_unit_config_validator.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/nr_cgi_helpers.h"
#include <map>
#include <set>

using namespace srsran;

static bool validate_mobility_appconfig(const gnb_id_t gnb_id, const mobility_unit_config& config)
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

  // check cu_cp_cell_config
  for (const auto& cell : config.cells) {
    std::set<nr_cell_id_t> ncis;
    if (ncis.emplace(cell.nr_cell_id).second == false) {
      fmt::print("Cells must be unique ({:#x} already present)\n");
      return false;
    }

    if (cell.ssb_period.has_value() && cell.ssb_offset.has_value() &&
        cell.ssb_offset.value() >= cell.ssb_period.value()) {
      fmt::print("ssb_offset must be smaller than ssb_period\n");
      return false;
    }

    // check that for the serving cell only periodic reports are configured
    if (cell.periodic_report_cfg_id.has_value()) {
      if (report_cfg_ids_to_report_type.at(cell.periodic_report_cfg_id.value()) != "periodical") {
        fmt::print("For the serving cell only periodic reports are allowed\n");
        return false;
      }
    }

    // Check if cell is an external managed cell
    if (config_helpers::get_gnb_id(cell.nr_cell_id, gnb_id.bit_length) != gnb_id) {
      if (!cell.gnb_id_bit_length.has_value() || !cell.pci.has_value() || !cell.band.has_value() ||
          !cell.ssb_arfcn.has_value() || !cell.ssb_scs.has_value() || !cell.ssb_period.has_value() ||
          !cell.ssb_offset.has_value() || !cell.ssb_duration.has_value()) {
        fmt::print(
            "For external cells, the gnb_id_bit_length, pci, band, ssb_argcn, ssb_scs, ssb_period, ssb_offset and "
            "ssb_duration must be configured in the mobility config\n");
        return false;
      }
    } else {
      if (cell.pci.has_value() || cell.band.has_value() || cell.ssb_arfcn.has_value() || cell.ssb_scs.has_value() ||
          cell.ssb_period.has_value() || cell.ssb_offset.has_value() || cell.ssb_duration.has_value()) {
        fmt::print("For cells managed by the CU-CP the gnb_id_bit_length, pci, band, ssb_argcn, ssb_scs, ssb_period, "
                   "ssb_offset and "
                   "ssb_duration must not be configured in the mobility config\n");
        return false;
      }
    }
  }

  return true;
}

/// Validates the given CU-CP configuration. Returns true on success, otherwise false.
static bool validate_cu_cp_appconfig(const gnb_id_t gnb_id, const cu_cp_unit_config& config)
{
  // validate mobility config
  if (!validate_mobility_appconfig(gnb_id, config.mobility_config)) {
    return false;
  }

  return true;
}

bool validate_cu_cp_unit_config(const cu_cp_unit_config& config)
{
  if (!validate_cu_cp_appconfig(config.gnb_id, config)) {
    return false;
  }

  return true;
}
