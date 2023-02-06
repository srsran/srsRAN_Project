#include "gnb_appconfig_validators.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/ran/phy_time_unit.h"
#include "srsgnb/srslog/logger.h"

using namespace srsgnb;

/// Validates the given RU application configuration. Returns true on success, otherwise false.
static bool validate_rf_driver_appconfig(const rf_driver_appconfig& config)
{
  static constexpr phy_time_unit reference_time = phy_time_unit::from_units_of_kappa(16);

  if (!reference_time.is_sample_accurate(config.srate_MHz * 1e6)) {
    fmt::print("The sampling rate must be multiple of {:.2f} MHz.\n", 1e-6 / reference_time.to_seconds());
    return false;
  }

  if (config.tx_time_advance_sps.has_value()) {
    unsigned srate_Hz = static_cast<unsigned>(config.srate_MHz * 1e6);

    // Compute the Tx time advance times in number of samples closest to the requested value that are aligned with an
    // integer number of NR-PHY time units. If the requested value is already aligned, the same value is returned.
    std::pair<int, int> time_adv_candidates =
        phy_time_unit::tc_aligned_nof_samples(config.tx_time_advance_sps.value(), srate_Hz);

    if (time_adv_candidates.first != config.tx_time_advance_sps) {
      // If the requested Tx time advance is not aligned, inform the user of the closest candidates.
      fmt::print(
          "Requested Tx time advance (i.e, {}) cannot be expressed in integer number of Tc (NR time units).\n"
          "Choose a sampling rate with a sampling period that is multiple of Tc, or set the time advance to one of the "
          "closest candidates: {} or {}.\n",
          config.tx_time_advance_sps,
          time_adv_candidates.first,
          time_adv_candidates.second);
      return false;
    }
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
  if (config.nof_antennas_dl != 1) {
    fmt::print("The requested number of DL antennas (i.e. {}) is invalid or not currently supported.\n",
               config.nof_antennas_dl);
    return false;
  }
  if (config.nof_antennas_ul != 1) {
    fmt::print("The requested number of UL antennas (i.e. {}) is invalid or not currently supported.\n",
               config.nof_antennas_ul);
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

bool srsgnb::validate_appconfig(const gnb_appconfig& config)
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
