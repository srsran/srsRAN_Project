#include "gnb_appconfig_validators.h"
#include "srsgnb/adt/span.h"

using namespace srsgnb;

/// Validates the given RU application configuration. Returns true on success, otherwise false.
static bool validate_rf_driver_appconfig(const rf_driver_appconfig& config)
{
  return true;
}

/// Validates the given PDCCH cell application configuration. Returns true on success, otherwise false.
static bool validate_pdcch_cell_app_config(const pdcch_appconfig& config)
{
  return true;
}

/// Validates the given PDSCH cell application configuration. Returns true on success, otherwise false.
static bool validate_pdsch_cell_app_config(const pdsch_appconfig& config)
{
  return true;
}

/// Validates the given CSI cell application configuration. Returns true on success, otherwise false.
static bool validate_csi_cell_app_config(const csi_appconfig& config)
{
  return true;
}

/// Validates the given PUCCH cell application configuration. Returns true on success, otherwise false.
static bool validate_pucch_cell_app_config(const pucch_appconfig& config)
{
  return true;
}

/// Validates the given PUSCH cell application configuration. Returns true on success, otherwise false.
static bool validate_pusch_cell_app_config(const pusch_appconfig& config)
{
  return true;
}

/// Validates the given PRACH cell application configuration. Returns true on success, otherwise false.
static bool validate_prach_cell_app_config(const prach_appconfig& config)
{
  return true;
}

/// Validates the given cell application configuration. Returns true on success, otherwise false.
static bool validate_base_cell_appconfig(const base_cell_appconfig& config)
{
  if (config.nof_antennas_dl != 1) {
    return false;
  }
  if (config.nof_antennas_ul != 1) {
    return false;
  }

  if (!validate_pdcch_cell_app_config(config.pdcch_cfg)) {
    return false;
  }

  if (!validate_pdsch_cell_app_config(config.pdsch_cfg)) {
    return false;
  }

  if (!validate_csi_cell_app_config(config.csi_cfg)) {
    return false;
  }

  if (!validate_pucch_cell_app_config(config.pucch_cfg)) {
    return false;
  }

  if (!validate_pusch_cell_app_config(config.pusch_cfg)) {
    return false;
  }

  if (!validate_prach_cell_app_config(config.prach_cfg)) {
    return false;
  }

  return true;
}

static bool validate_cells_appconfig(const cell_appconfig& config)
{
  if (config.pci > 1007) {
    return false;
  }

  return validate_base_cell_appconfig(config.cell);
}

/// Validates the given list of cell application configuration. Returns true on success, otherwise false.
static bool validate_cells_appconfig(span<const cell_appconfig> config)
{
  // Currently supporting one cell.
  if (config.size() != 1) {
    return false;
  }

  for (const auto& cell : config) {
    if (!validate_cells_appconfig(cell)) {
      return false;
    }
  }

  return true;
}

/// Validates the given CU application configuration. Returns true on success, otherwise false.
static bool validate_amf_appconfig(const amf_appconfig& config)
{
  return true;
}

bool srsgnb::validate_appconfig(const gnb_appconfig& config)
{
  if (config.log_level != "info" && config.log_level != "debug" && config.log_level != "warning" &&
      config.log_level != "error") {
    return false;
  }

  if (!validate_amf_appconfig(config.amf_cfg)) {
    return false;
  }

  if (!validate_cells_appconfig(config.cells_cfg)) {
    return false;
  }

  if (!validate_rf_driver_appconfig(config.rf_driver_cfg)) {
    return false;
  }

  return true;
}
