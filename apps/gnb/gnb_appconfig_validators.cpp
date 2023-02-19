#include "gnb_appconfig_validators.h"
#include "srsran/adt/span.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/srslog/logger.h"

using namespace srsran;

/// Validates the given RU application configuration. Returns true on success, otherwise false.
static bool validate_rf_driver_appconfig(const rf_driver_appconfig& config)
{
  static constexpr phy_time_unit reference_time = phy_time_unit::from_units_of_kappa(16);

  if (!reference_time.is_sample_accurate(config.srate_MHz * 1e6)) {
    fmt::print("The sampling rate must be multiple of {:.2f} MHz.\n", 1e-6 / reference_time.to_seconds());
    return false;
  }

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
  // The maximum value of 137 for \c l139 is defined by \c prach-RootSequenceIndex, in RACH-ConfigCommon, TS 38.331.
  // The lengths \c l839 or \c l139 depend on the PRACH preamble format, defined in Tables 6.3.3.1-1 and
  // Table 6.3.3.1-2, TS 38.211. \n
  // The PRACH preamble format depends on prach_root_sequence_index, as per Tables Table 6.3.3.2-2 (FR1 and paired
  // spectrum) and Table 6.3.3.2-3 (FR1 and unpaired spectrum), TS 38.211.
  // NOTE: This check is condition on whether the GNB uses paired or unpaired spectrum; since this information is not
  // available here, in unpaired spectrum there are some invalid combinations of prach_config_index and
  // prach_root_sequence_index that are not rejected by this validator.
  if (config.prach_config_index > 86 and config.prach_root_sequence_index > 137) {
    fmt::print("The PRACH preamble format for PRACH configuration index {} is not compatible with Root seq. idx. {}. "
               "The max Root seq. idx. for this PRACH configuration index is 137 \n",
               config.prach_config_index,
               config.prach_root_sequence_index);
    return false;
  }

  return true;
}

/// Validates the given amplitude control application configuration. Returns true on success, otherwise false.
static bool validate_amplitude_control_appconfig(const amplitude_control_appconfig& config)
{
  bool valid = true;

  if (config.gain_backoff_dB < 0.0F) {
    fmt::print("Invalid gain backoff (i.e., {:.1f} dB). Must be positive or zero.\n", config.gain_backoff_dB);
    valid = false;
  }
  if (config.power_ceiling_dBFS > 0.0F) {
    fmt::print("Invalid power ceiling (i.e., {:.1f} dBFS). Must be negative or zero.\n", config.power_ceiling_dBFS);
    valid = false;
  }

  return valid;
}

/// Validates the given cell application configuration. Returns true on success, otherwise false.
static bool validate_base_cell_appconfig(const base_cell_appconfig& config)
{
  if (config.nof_antennas_dl == 0) {
    fmt::print("The number of DL antennas cannot be zero.");
    return false;
  }
  if (config.nof_antennas_ul == 0) {
    fmt::print("The number of UL antennas cannot be zero.");
    return false;
  }
  if (config.nof_antennas_dl != 1) {
    fmt::print("Currently, only one DL antenna is supported.\n", config.nof_antennas_dl);
    return false;
  }
  if (config.nof_antennas_ul != 1) {
    fmt::print("Currently, only one UL antenna is supported.\n", config.nof_antennas_ul);
    return false;
  }
  if (config.band.has_value()) {
    error_type<std::string> ret = band_helper::is_dl_arfcn_valid_given_band(*config.band, config.dl_arfcn);
    if (ret.is_error()) {
      fmt::print("Invalid DL ARFCN={} for band {}. Cause: {}.\n", config.dl_arfcn, *config.band, ret.error());
      return false;
    }
  } else {
    if (band_helper::get_band_from_dl_arfcn(config.dl_arfcn) == nr_band::invalid) {
      fmt::print("Invalid DL ARFCN={}. Cause: Could not find a valid band.\n", config.dl_arfcn);
      return false;
    }
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

  if (!validate_amplitude_control_appconfig(config.amplitude_cfg)) {
    return false;
  }

  return true;
}

static bool validate_cells_appconfig(const cell_appconfig& config)
{
  if (config.pci > 1007) {
    fmt::print("Invalid PCI (i.e. {}). PCI ranges from 0 to {}.\n", config.pci, MAX_PCI);
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

/// Validates the given AMF configuration. Returns true on success, otherwise false.
static bool validate_amf_appconfig(const amf_appconfig& config)
{
  // only check for non-empty AMF address and default port
  if (config.ip_addr.empty() or config.port != 38412) {
    return false;
  }
  return true;
}

/// Validates the given QoS configuration. Returns true on success, otherwise false.
static bool validate_qos_appconfig(span<const qos_appconfig> config)
{
  // TODO check validity of QoS configs
  return true;
}

/// Validates the given logging configuration. Returns true on success, otherwise false.
static bool validate_log_appconfig(const log_appconfig& config)
{
  if (config.filename.empty()) {
    return false;
  }

  // validate level selection
  if (srslog::str_to_basic_level(config.app_level) == srslog::basic_levels::none) {
    return false;
  }

  if (srslog::str_to_basic_level(config.du_level) == srslog::basic_levels::none) {
    return false;
  }

  if (srslog::str_to_basic_level(config.cu_level) == srslog::basic_levels::none) {
    return false;
  }

  if (srslog::str_to_basic_level(config.phy_level) == srslog::basic_levels::none) {
    return false;
  }

  if (srslog::str_to_basic_level(config.mac_level) == srslog::basic_levels::none) {
    return false;
  }

  if (srslog::str_to_basic_level(config.rlc_level) == srslog::basic_levels::none) {
    return false;
  }

  if (srslog::str_to_basic_level(config.pdcp_level) == srslog::basic_levels::none) {
    return false;
  }

  if (srslog::str_to_basic_level(config.rrc_level) == srslog::basic_levels::none) {
    return false;
  }

  return true;
}

/// Validates expert physical layer configuration parameters.
static bool validate_expert_phy_appconfig(const expert_phy_appconfig& config)
{
  if (config.pusch_decoder_max_iterations == 0) {
    fmt::print("Maximum PUSCH LDPC decoder iterations cannot be 0.\n");
    return false;
  }
  return true;
}

bool srsran::validate_appconfig(const gnb_appconfig& config)
{
  if (!validate_log_appconfig(config.log_cfg)) {
    return false;
  }

  if (!validate_amf_appconfig(config.amf_cfg)) {
    return false;
  }

  if (!validate_cells_appconfig(config.cells_cfg)) {
    return false;
  }

  if (!validate_qos_appconfig(config.qos_cfg)) {
    return false;
  }

  if (!validate_rf_driver_appconfig(config.rf_driver_cfg)) {
    return false;
  }

  if (!validate_expert_phy_appconfig(config.expert_phy_cfg)) {
    return false;
  }

  if (config.rf_driver_cfg.srate_MHz < bs_channel_bandwidth_to_MHz(config.common_cell_cfg.channel_bw_mhz)) {
    fmt::print("Sampling rate (i.e. {} MHz) is too low for the requested channel bandwidth (i.e. {} MHz).\n",
               config.rf_driver_cfg.srate_MHz,
               bs_channel_bandwidth_to_MHz(config.common_cell_cfg.channel_bw_mhz));
    return false;
  }

  return true;
}
