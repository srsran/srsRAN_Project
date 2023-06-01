/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "gnb_appconfig_validators.h"
#include "srsran/adt/span.h"
#include "srsran/adt/variant.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/srslog/logger.h"

using namespace srsran;

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

/// Validates the given SDR Radio Unit application configuration. Returns true on success, otherwise false.
static bool validate_ru_sdr_appconfig(const ru_sdr_appconfig& config)
{
  static constexpr phy_time_unit reference_time = phy_time_unit::from_units_of_kappa(16);

  if (!reference_time.is_sample_accurate(config.srate_MHz * 1e6)) {
    fmt::print("The sampling rate must be multiple of {:.2f} MHz.\n", 1e-6 / reference_time.to_seconds());
    return false;
  }

  for (const auto& cell : config.cells) {
    if (!validate_amplitude_control_appconfig(cell.amplitude_cfg)) {
      return false;
    }
  }

  return true;
}

/// Validates the given Open Fronthaul Radio Unit application configuration. Returns true on success, otherwise false.
static bool validate_ru_ofh_appconfig(const ru_ofh_appconfig& config)
{
  for (const auto& cell : config.cells) {
    std::vector<unsigned> ports = cell.ru_dl_port_id;
    std::sort(ports.begin(), ports.end());

    if (std::unique(ports.begin(), ports.end()) != ports.end()) {
      fmt::print("Detected duplicated downlink port identifiers\n");

      return false;
    }
  }

  return true;
}

static bool validate_rv_sequence(span<const unsigned> rv_sequence)
{
  if (rv_sequence.empty()) {
    fmt::print("Invalid RV sequence. The RV sequence cannot be empty.\n");
    return false;
  }
  if (rv_sequence[0] != 0) {
    fmt::print("Invalid RV sequence. The first RV must be 0.\n");
    return false;
  }
  return true;
}

/// Validates the given PDSCH cell application configuration. Returns true on success, otherwise false.
static bool validate_pdsch_cell_app_config(const pdsch_appconfig& config)
{
  if (config.min_ue_mcs > config.max_ue_mcs) {
    fmt::print("Invalid UE MCS range (i.e., [{}, {}]). The min UE MCS must be less than or equal to the max UE MCS.\n",
               config.min_ue_mcs,
               config.max_ue_mcs);
    return false;
  }

  if (not validate_rv_sequence(config.rv_sequence)) {
    return false;
  }

  return true;
}

/// Validates the given PUSCH cell application configuration. Returns true on success, otherwise false.
static bool validate_pusch_cell_app_config(const pusch_appconfig& config)
{
  if (config.min_ue_mcs > config.max_ue_mcs) {
    fmt::print("Invalid UE MCS range (i.e., [{}, {}]). The min UE MCS must be less than or equal to the max UE MCS.\n",
               config.min_ue_mcs,
               config.max_ue_mcs);
    return false;
  }

  if (not validate_rv_sequence(config.rv_sequence)) {
    return false;
  }

  return true;
}

/// Validates the given PRACH cell application configuration. Returns true on success, otherwise false.
static bool validate_prach_cell_app_config(const prach_appconfig& config, nr_band band)
{
  bool       is_paired_spectrum = band_helper::is_paired_spectrum(band);
  const bool is_prach_cfg_idx_supported =
      is_paired_spectrum
          ? config.prach_config_index <= 107U or (config.prach_config_index > 197U and config.prach_config_index < 219U)
          : config.prach_config_index <= 86U or (config.prach_config_index > 144U and config.prach_config_index < 169U);
  if (not is_prach_cfg_idx_supported) {
    fmt::print("PRACH configuration index {} not supported. For {}, the supported PRACH configuration indices are {}\n",
               config.prach_config_index,
               is_paired_spectrum ? "FDD" : "TDD",
               is_paired_spectrum ? "[0, 107] and [198, 218]" : "[0, 86] and [145, 168]");
    return false;
  }

  return true;
}

/// Validates the given TDD UL DL pattern application configuration. Returns true on success, otherwise false.
static bool validate_tdd_ul_dl_appconfig(const tdd_ul_dl_appconfig& config, subcarrier_spacing common_scs)
{
  // NOTE: TDD pattern is assumed to use common SCS as reference SCS.
  if (common_scs > subcarrier_spacing::kHz60) {
    fmt::print("Invalid TDD UL DL reference SCS={}. Must be 15, 30 or 60 kHz for FR1.\n", common_scs);
    return false;
  }
  if (config.dl_ul_tx_period != 0.5F and config.dl_ul_tx_period != 0.625F and config.dl_ul_tx_period != 1.0F and
      config.dl_ul_tx_period != 1.25F and config.dl_ul_tx_period != 2.0F and config.dl_ul_tx_period != 2.5F and
      config.dl_ul_tx_period != 5.0F and config.dl_ul_tx_period != 10.0F) {
    fmt::print(
        "Invalid TDD pattern 1 UL DL periodicity={}ms. Must be 0.5, 0.625, 1, 1.25, 2, 2.5, 5 or 10 milliseconds.\n",
        config.dl_ul_tx_period);
    return false;
  }
  // See TS 38.213, clause 11.1.
  if (config.dl_ul_tx_period == 0.625F and common_scs != subcarrier_spacing::kHz120) {
    fmt::print("Invalid reference SCS={} for TDD pattern 1. Must be 120 kHz when using "
               "periodicity of {} ms.\n",
               common_scs,
               config.dl_ul_tx_period);
    return false;
  }
  if (config.dl_ul_tx_period == 1.25F and
      (common_scs != subcarrier_spacing::kHz120 and common_scs != subcarrier_spacing::kHz60)) {
    fmt::print("Invalid reference SCS={} for TDD pattern 1. Must be 120 or 60 kHz when using "
               "periodicity of {} ms.\n",
               common_scs,
               config.dl_ul_tx_period);
    return false;
  }
  if (config.dl_ul_tx_period == 2.5F and
      (common_scs != subcarrier_spacing::kHz120 and common_scs != subcarrier_spacing::kHz60 and
       common_scs != subcarrier_spacing::kHz30)) {
    fmt::print("Invalid reference SCS={} for TDD pattern 1. Must be 120, 60 or 30 kHz when using "
               "periodicity of {} ms.\n",
               common_scs,
               config.dl_ul_tx_period);
    return false;
  }

  return true;
}

static bool validate_dl_arfcn_and_band(const base_cell_appconfig& config)
{
  nr_band band = config.band.has_value() ? config.band.value() : band_helper::get_band_from_dl_arfcn(config.dl_arfcn);

  // Check if the band is supported with given SCS or band.
  // NOTE: Band n46 would be compatible with the 10MHz BW, but there is no sync raster that falls within the band
  // limits. Also, the Coreset#0 width in RBs given in Table 13-4A, TS 38.213, is larger than the band itself, which is
  // odd. Therefore, we limit the band to minimum 20MHz BW.
  if (band == srsran::nr_band::n46 and config.channel_bw_mhz < bs_channel_bandwidth_fr1::MHz20) {
    fmt::print("Minimum supported bandwidth for n46 is 20MHz.\n");
    return false;
  }

  if (bs_channel_bandwidth_to_MHz(config.channel_bw_mhz) <
      min_channel_bandwidth_to_MHz(band_helper::get_min_channel_bw(band, config.common_scs))) {
    fmt::print("Minimum supported bandwidth for n{} with SCS {} is {}MHz.\n",
               config.band,
               to_string(config.common_scs),
               min_channel_bandwidth_to_MHz(band_helper::get_min_channel_bw(band, config.common_scs)));
    return false;
  }

  // Check whether the DL-ARFCN is within the band and follows the Raster step.
  if (config.band.has_value()) {
    error_type<std::string> ret = band_helper::is_dl_arfcn_valid_given_band(
        *config.band, config.dl_arfcn, config.common_scs, config.channel_bw_mhz);
    if (ret.is_error()) {
      fmt::print("Invalid DL ARFCN={} for band {}. Cause: {}.\n", config.dl_arfcn, band, ret.error());
      return false;
    }
  } else {
    if (band == nr_band::invalid) {
      fmt::print("Invalid DL ARFCN={}. Cause: Could not find a valid band.\n", config.dl_arfcn);
      return false;
    }
  }

  return true;
}

/// Validates the given cell application configuration. Returns true on success, otherwise false.
static bool validate_base_cell_appconfig(const base_cell_appconfig& config)
{
  if (config.pci >= INVALID_PCI) {
    fmt::print("Invalid PCI (i.e. {}). PCI ranges from 0 to {}.\n", config.pci, MAX_PCI);
    return false;
  }
  if (config.nof_antennas_dl == 0) {
    fmt::print("The number of DL antennas cannot be zero.\n");
    return false;
  }
  if (config.nof_antennas_ul == 0) {
    fmt::print("The number of UL antennas cannot be zero.\n");
    return false;
  }
  if (config.nof_antennas_ul != config.nof_antennas_dl) {
    fmt::print("Different number of UL and DL antennas is not currently supported.\n");
    return false;
  }
  if (config.common_scs == srsran::subcarrier_spacing::kHz15 and
      config.channel_bw_mhz > srsran::bs_channel_bandwidth_fr1::MHz50) {
    fmt::print("Maximum Channel BW with SCS common 15kHz is 50MHz.\n");
    return false;
  }
  if (config.common_scs == srsran::subcarrier_spacing::kHz30 and
      config.channel_bw_mhz < srsran::bs_channel_bandwidth_fr1::MHz10) {
    fmt::print("Minimum supported Channel BW with SCS common 30kHz is 10MHz.\n");
    return false;
  }

  if (!validate_dl_arfcn_and_band(config)) {
    return false;
  }

  if (!validate_pdsch_cell_app_config(config.pdsch_cfg)) {
    return false;
  }

  if (!validate_pusch_cell_app_config(config.pusch_cfg)) {
    return false;
  }

  nr_band band = config.band.has_value() ? config.band.value() : band_helper::get_band_from_dl_arfcn(config.dl_arfcn);
  if (!validate_prach_cell_app_config(config.prach_cfg, band)) {
    return false;
  }

  if (!band_helper::is_paired_spectrum(band) and config.tdd_ul_dl_cfg.has_value() and
      !validate_tdd_ul_dl_appconfig(config.tdd_ul_dl_cfg.value(), config.common_scs)) {
    return false;
  }

  return true;
}

static bool validate_cells_appconfig(const cell_appconfig& config)
{
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

/// Validates the given PDCP configuration. Returns true on success, otherwise false.
static bool validate_pdcp_appconfig(five_qi_t five_qi, const pdcp_appconfig& config)
{
  if (config.integrity_protection_required) {
    fmt::print("PDCP DRB integrity protection is not supported yet. 5QI={}\n", five_qi);
    return false;
  }

  // Check TX
  if (config.tx.sn_field_length != 12 && config.tx.sn_field_length != 18) {
    fmt::print("PDCP TX SN length is neither 12 or 18 bits. 5QI={} SN={}\n", five_qi, config.tx.sn_field_length);
    return false;
  }
  if (config.tx.status_report_required) {
    fmt::print("PDCP TX status report required not supported yet. 5QI={}\n", five_qi);
    return false;
  }

  // Check RX
  if (config.rx.sn_field_length != 12 && config.rx.sn_field_length != 18) {
    fmt::print("PDCP RX SN length is neither 12 or 18 bits. 5QI={} SN={}\n", five_qi, config.rx.sn_field_length);
    return false;
  }

  pdcp_t_reordering t_reordering = {};
  if (!pdcp_t_reordering_from_int(t_reordering, config.rx.t_reordering)) {
    fmt::print("PDCP RX t-Reordering is not a valid value. 5QI={}, t-Reordering={}\n", five_qi, config.rx.t_reordering);
    fmt::print("Valid values: "
               "\"infinity, ms0, ms1, ms2, ms4, ms5, ms8, ms10, ms15, ms20, ms30, ms40,ms50, ms60, ms80, "
               "ms100, ms120, ms140, ms160, ms180, ms200, ms220,ms240, ms260, ms280, ms300, ms500, ms750, ms1000, "
               "ms1250, ms1500, ms1750, ms2000, ms2250, ms2500, ms2750\"\n");
    return false;
  }
  if (t_reordering == pdcp_t_reordering::infinity) {
    srslog::basic_logger& logger = srslog::fetch_basic_logger("GNB");
    fmt::print("PDCP t-Reordering=infinity on DRBs is not advised. It can cause data stalls. 5QI={}\n", five_qi);
    logger.warning("PDCP t-Reordering=infinity on DRBs is not advised. It can cause data stalls. 5QI={}", five_qi);
  }

  if (config.rx.out_of_order_delivery) {
    fmt::print("PDCP RX out-of-order delivery is not supported. 5QI={}\n", five_qi);
    return false;
  }
  return true;
}

/// Validates the given QoS configuration. Returns true on success, otherwise false.
static bool validate_qos_appconfig(span<const qos_appconfig> config)
{
  for (const auto& qos : config) {
    if (!validate_pdcp_appconfig(qos.five_qi, qos.pdcp)) {
      return false;
    }
  }
  return true;
}

/// Validates the given logging configuration. Returns true on success, otherwise false.
static bool validate_log_appconfig(const log_appconfig& config)
{
  if (config.filename.empty()) {
    return false;
  }

  // validate level selection
  if (srslog::str_to_basic_level(config.all_level) == srslog::basic_levels::none) {
    return false;
  }

  if (srslog::str_to_basic_level(config.lib_level) == srslog::basic_levels::none) {
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

  if (srslog::str_to_basic_level(config.radio_level) == srslog::basic_levels::none) {
    return false;
  }

  if (srslog::str_to_basic_level(config.fapi_level) == srslog::basic_levels::none) {
    return false;
  }

  if (srslog::str_to_basic_level(config.ofh_level) == srslog::basic_levels::none) {
    return false;
  }

  return true;
}

/// Validates expert physical layer configuration parameters.
static bool validate_expert_phy_appconfig(const expert_upper_phy_appconfig& config)
{
  static const interval<unsigned>  nof_ul_threads_range(1, std::thread::hardware_concurrency());
  static constexpr interval<float> lphy_dl_throttling_range(0.0F, 1.0F);

  bool valid = true;

  if (!nof_ul_threads_range.contains(config.nof_ul_threads)) {
    fmt::print("Number of UL threads (i.e., {}) must be in range {}.\n", config.nof_ul_threads, nof_ul_threads_range);
    valid = false;
  }

  if (config.pusch_decoder_max_iterations == 0) {
    fmt::print("Maximum PUSCH LDPC decoder iterations cannot be zero.\n");
    valid = false;
  }

  if (!lphy_dl_throttling_range.contains(config.lphy_dl_throttling)) {
    fmt::print(
        "Low PHY throttling (i.e., {}) must be in range {}.\n", config.lphy_dl_throttling, lphy_dl_throttling_range);
    valid = false;
  }

  return valid;
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

  if (!validate_expert_phy_appconfig(config.expert_phy_cfg)) {
    return false;
  }

  if (variant_holds_alternative<ru_sdr_appconfig>(config.ru_cfg)) {
    const ru_sdr_appconfig& sdr_cfg = variant_get<ru_sdr_appconfig>(config.ru_cfg);

    if (config.cells_cfg.size() != sdr_cfg.cells.size()) {
      fmt::print("Number of cells in the DU={} don't match the number of cells in the RU={}\n",
                 config.cells_cfg.size(),
                 sdr_cfg.cells.size());

      return false;
    }

    if (!validate_ru_sdr_appconfig(sdr_cfg)) {
      return false;
    }

    if (sdr_cfg.srate_MHz < bs_channel_bandwidth_to_MHz(config.common_cell_cfg.channel_bw_mhz)) {
      fmt::print("Sampling rate (i.e. {} MHz) is too low for the requested channel bandwidth (i.e. {} MHz).\n",
                 sdr_cfg.srate_MHz,
                 bs_channel_bandwidth_to_MHz(config.common_cell_cfg.channel_bw_mhz));
      return false;
    }
  }

  if (variant_holds_alternative<ru_ofh_appconfig>(config.ru_cfg)) {
    const ru_ofh_appconfig& ofh_cfg = variant_get<ru_ofh_appconfig>(config.ru_cfg);

    if (config.cells_cfg.size() != ofh_cfg.cells.size()) {
      fmt::print("Number of cells in the DU={} don't match the number of cells in the RU={}\n",
                 config.cells_cfg.size(),
                 ofh_cfg.cells.size());

      return false;
    }

    if (!validate_ru_ofh_appconfig(ofh_cfg)) {
      return false;
    }
  }

  if (config.gnb_id_bit_length < 22 or config.gnb_id_bit_length > 32) {
    fmt::print("gNB id bit length must be within the range [22,..,32].\n");
    return false;
  }

  return true;
}
