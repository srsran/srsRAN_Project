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
#include "srsran/ran/band_helper.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/prach/prach_configuration.h"
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

  static constexpr interval<float> lphy_dl_throttling_range(0.0F, 1.0F);
  if (!lphy_dl_throttling_range.contains(config.expert_cfg.lphy_dl_throttling)) {
    fmt::print("Low PHY throttling (i.e., {}) must be in range {}.\n",
               config.expert_cfg.lphy_dl_throttling,
               lphy_dl_throttling_range);
    return false;
  }

  for (const auto& cell : config.cells) {
    if (!validate_amplitude_control_appconfig(cell.amplitude_cfg)) {
      return false;
    }
  }

  return true;
}

/// Validates that the given Radio Unit ports are not duplicated. Returns true on success, otherwise false.
static bool validate_ru_duplicated_ports(const std::vector<unsigned>& ru_ports)
{
  std::vector<unsigned> ports = ru_ports;
  std::sort(ports.begin(), ports.end());

  return std::unique(ports.begin(), ports.end()) == ports.end();
}

/// Validates the given Open Fronthaul Radio Unit application configuration. Returns true on success, otherwise
/// false.
static bool validate_ru_ofh_appconfig(const gnb_appconfig& config)
{
  const ru_ofh_appconfig& ofh_cfg = variant_get<ru_ofh_appconfig>(config.ru_cfg);

  for (unsigned i = 0, e = config.cells_cfg.size(); i != e; ++i) {
    const ru_ofh_cell_appconfig& ofh_cell = ofh_cfg.cells[i];
    const base_cell_appconfig&   cell_cfg = config.cells_cfg[i].cell;

    if (!ofh_cell.cell.is_downlink_broadcast_enabled && cell_cfg.nof_antennas_dl != ofh_cell.ru_dl_port_id.size()) {
      fmt::print("RU number of downlink ports={} must match the number of transmission antennas={}\n",
                 ofh_cell.ru_dl_port_id.size(),
                 cell_cfg.nof_antennas_dl);

      return false;
    }

    if (!validate_ru_duplicated_ports(ofh_cell.ru_dl_port_id)) {
      fmt::print("Detected duplicated downlink port identifiers\n");

      return false;
    }

    if (!validate_ru_duplicated_ports(ofh_cell.ru_ul_port_id)) {
      fmt::print("Detected duplicated uplink port identifiers\n");

      return false;
    }

    if (!validate_ru_duplicated_ports(ofh_cell.ru_prach_port_id)) {
      fmt::print("Detected duplicated uplink PRACH port identifiers\n");

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

  prach_configuration prach_config =
      prach_configuration_get(frequency_range::FR1, band_helper::get_duplex_mode(band), config.prach_config_index);
  if (is_paired_spectrum && (prach_config.format == prach_format_type::B4) && (config.zero_correlation_zone != 0) &&
      (config.zero_correlation_zone != 11)) {
    fmt::print("PRACH Zero Correlation Zone index (i.e., {}) with Format B4 is not supported for FDD. Use 0 or 11.\n",
               config.zero_correlation_zone);
    return false;
  }
  if (!is_paired_spectrum && (prach_config.format == prach_format_type::B4) && (config.zero_correlation_zone != 0) &&
      (config.zero_correlation_zone != 14)) {
    fmt::print("PRACH Zero Correlation Zone index (i.e., {}) with Format B4 is not supported for FDD. Use 0 or 14.\n",
               config.zero_correlation_zone);
    return false;
  }

  return true;
}

static bool validate_tdd_ul_dl_pattern_appconfig(const tdd_ul_dl_pattern_appconfig& config,
                                                 subcarrier_spacing                 common_scs)
{
  // NOTE: TDD pattern is assumed to use common SCS as reference SCS.
  if (common_scs > subcarrier_spacing::kHz60) {
    fmt::print("Invalid TDD UL DL reference SCS={}kHz. Must be 15, 30 or 60 kHz for FR1.\n", scs_to_khz(common_scs));
    return false;
  }

  const unsigned period_msec = config.dl_ul_period_slots / get_nof_slots_per_subframe(common_scs);

  static constexpr std::array<float, 10> valid_periods = {0.5F, 0.625, 1.0, 1.25, 2.0, 2.5, 3.0, 4.0, 5.0, 10.0};
  if (std::none_of(valid_periods.begin(), valid_periods.end(), [period_msec](float v) {
        return std::abs(period_msec - v) < 1e-3;
      })) {
    fmt::print(
        "Invalid TDD pattern periodicity={}ms. It must be in {{{}}}.\n", period_msec, fmt::join(valid_periods, ", "));
    return false;
  }

  return true;
}

/// Validates the given TDD UL DL pattern application configuration. Returns true on success, otherwise false.
static bool validate_tdd_ul_dl_appconfig(const tdd_ul_dl_appconfig& config, subcarrier_spacing common_scs)
{
  if (not validate_tdd_ul_dl_pattern_appconfig(config.pattern1, common_scs)) {
    return false;
  }
  if (config.pattern2.has_value() and not validate_tdd_ul_dl_pattern_appconfig(config.pattern2.value(), common_scs)) {
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

  if (band_helper::get_duplex_mode(band) == duplex_mode::TDD and config.tdd_ul_dl_cfg.has_value() and
      !validate_tdd_ul_dl_appconfig(config.tdd_ul_dl_cfg.value(), config.common_scs)) {
    return false;
  }

  if (config.pdsch_cfg.nof_ports.has_value() and config.nof_antennas_dl < *config.pdsch_cfg.nof_ports) {
    fmt::print("Number of PDSCH ports {} cannot be higher than the number of DL antennas {}\n",
               *config.pdsch_cfg.nof_ports,
               config.nof_antennas_dl);
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
  static const interval<unsigned, true> nof_ul_dl_threads_range(1, std::thread::hardware_concurrency());

  bool valid = true;

  if (!nof_ul_dl_threads_range.contains(config.nof_ul_threads)) {
    fmt::print(
        "Number of PHY UL threads (i.e., {}) must be in range {}.\n", config.nof_ul_threads, nof_ul_dl_threads_range);
    valid = false;
  }

  if (!nof_ul_dl_threads_range.contains(config.nof_pdsch_threads)) {
    fmt::print("Number of PHY PDSCH threads (i.e., {}) must be in range {}.\n",
               config.nof_pdsch_threads,
               nof_ul_dl_threads_range);
    valid = false;
  }

  if (!nof_ul_dl_threads_range.contains(config.nof_dl_threads)) {
    fmt::print(
        "Number of PHY DL threads (i.e., {}) must be in range {}.\n", config.nof_dl_threads, nof_ul_dl_threads_range);
    valid = false;
  }

  if (config.nof_dl_threads > config.max_processing_delay_slots) {
    fmt::print(
        "Number of PHY DL threads (i.e., {}) cannot be larger than the maximum processing delay in slots (i.e., {}).\n",
        config.nof_dl_threads,
        config.max_processing_delay_slots);
    valid = false;
  }

  if (config.pusch_decoder_max_iterations == 0) {
    fmt::print("Maximum PUSCH LDPC decoder iterations cannot be zero.\n");
    valid = false;
  }

  return valid;
}

static bool validate_pdcch_appconfig(const gnb_appconfig& config)
{
  for (const auto& cell : config.cells_cfg) {
    const base_cell_appconfig& base_cell = cell.cell;
    const auto                 band =
        base_cell.band.has_value() ? *base_cell.band : band_helper::get_band_from_dl_arfcn(base_cell.dl_arfcn);
    const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
        base_cell.channel_bw_mhz, base_cell.common_scs, band_helper::get_freq_range(band));
    if (base_cell.pdcch_cfg.common.coreset0_index.has_value()) {
      const uint8_t  ss0_idx               = base_cell.pdcch_cfg.common.ss0_index;
      const unsigned cs0_idx               = base_cell.pdcch_cfg.common.coreset0_index.value();
      const auto     ssb_coreset0_freq_loc = band_helper::get_ssb_coreset0_freq_location(
          base_cell.dl_arfcn, band, nof_crbs, base_cell.common_scs, base_cell.common_scs, ss0_idx, cs0_idx);
      if (not ssb_coreset0_freq_loc.has_value()) {
        fmt::print("Unable to derive a valid SSB pointA and k_SSB for CORESET#0 index={}, SearchSpace#0 index={} and "
                   "cell bandwidth={}Mhz\n",
                   cs0_idx,
                   ss0_idx,
                   base_cell.channel_bw_mhz);
        return false;
      }
      // NOTE: The CORESET duration of 3 symbols is only permitted if the dmrs-typeA-Position information element has
      // been set to 3. And, we use only pos2 or pos1
      const pdcch_type0_css_coreset_description desc = srsran::pdcch_type0_css_coreset_get(
          band, base_cell.common_scs, base_cell.common_scs, cs0_idx, ssb_coreset0_freq_loc->k_ssb.to_uint());
      if (desc.pattern != PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern and desc.nof_symb_coreset == 3) {
        fmt::print("CORESET duration of 3 OFDM symbols corresponding to CORESET#0 index={} is not supported\n",
                   cs0_idx);
        return false;
      }
    }
    if (base_cell.pdcch_cfg.dedicated.coreset1_rb_start.has_value() and
        base_cell.pdcch_cfg.dedicated.coreset1_rb_start.value() > nof_crbs) {
      fmt::print("Invalid CORESET#1 RBs start={}\n", base_cell.pdcch_cfg.dedicated.coreset1_rb_start.value());
      return false;
    }
    if (base_cell.pdcch_cfg.dedicated.coreset1_l_crb.has_value() and
        base_cell.pdcch_cfg.dedicated.coreset1_l_crb.value() > nof_crbs) {
      fmt::print("Invalid CORESET#1 length in RBs={}\n", base_cell.pdcch_cfg.dedicated.coreset1_l_crb.value());
      return false;
    }
    if (base_cell.pdcch_cfg.dedicated.coreset1_rb_start.has_value() and
        base_cell.pdcch_cfg.dedicated.coreset1_l_crb.has_value() and
        base_cell.pdcch_cfg.dedicated.coreset1_rb_start.value() + base_cell.pdcch_cfg.dedicated.coreset1_l_crb.value() >
            nof_crbs) {
      fmt::print("CORESET#1 range={{},{}} outside of cell bandwidth={}CRBs\n",
                 base_cell.pdcch_cfg.dedicated.coreset1_rb_start.value(),
                 base_cell.pdcch_cfg.dedicated.coreset1_rb_start.value() +
                     base_cell.pdcch_cfg.dedicated.coreset1_l_crb.value(),
                 nof_crbs);
      return false;
    }
    if (config.common_cell_cfg.pdcch_cfg.dedicated.ss2_type == search_space_configuration::type_t::common and
        config.common_cell_cfg.pdcch_cfg.dedicated.dci_format_0_1_and_1_1) {
      fmt::print("Non-fallback DCI format not allowed in Common SearchSpace\n");
      return false;
    }
    if (not config.common_cell_cfg.pdcch_cfg.dedicated.dci_format_0_1_and_1_1 and
        base_cell.pdcch_cfg.dedicated.coreset1_rb_start.has_value() and
        base_cell.pdcch_cfg.dedicated.coreset1_rb_start.value() == 0) {
      // [Implementation-defined] Reason for starting from frequency resource 1 (i.e. CRB6) to remove the ambiguity of
      // UE decoding the DCI in CSS rather than USS when using fallback DCI formats (DCI format 1_0 and 0_0).
      fmt::print("Cannot start CORESET#1 from CRB0 in Common SearchSpace\n");
      return false;
    }
  }
  return true;
}

static bool validate_test_mode_appconfig(const gnb_appconfig& config)
{
  if ((config.test_mode_cfg.test_ue.ri > 1) and not config.common_cell_cfg.pdcch_cfg.dedicated.dci_format_0_1_and_1_1) {
    fmt::print("For test mode, RI shall not be set if UE is configured to use DCI format 1_0\n");
    return false;
  }
  unsigned nof_ports = config.common_cell_cfg.pdsch_cfg.nof_ports.has_value()
                           ? *config.common_cell_cfg.pdsch_cfg.nof_ports
                           : config.common_cell_cfg.nof_antennas_dl;
  if (config.test_mode_cfg.test_ue.ri > nof_ports) {
    fmt::print("For test mode, RI cannot be higher than the number of DL antenna ports ({} > {})\n",
               config.test_mode_cfg.test_ue.ri,
               config.common_cell_cfg.pdsch_cfg.nof_ports);
    return false;
  }

  if (config.test_mode_cfg.test_ue.ri != 2 and config.test_mode_cfg.test_ue.i_1_3 != 0) {
    fmt::print("For test mode with 4 antennas, i_1_3 can only be set for RI=2\n");
  }

  if (config.test_mode_cfg.test_ue.ri != 1 and config.test_mode_cfg.test_ue.i_2 > 1) {
    fmt::print("For test mode with 4 antennas, i_2 can only be higher than 1 for RI=1\n");
  }

  return true;
}

static bool validate_ntn_config(const ntn_config ntn_cfg)
{
  bool valid = true;

  if (ntn_cfg.cell_specific_koffset > 1023) {
    fmt::print("Cell specific koffset must be in range [0, 1023].\n");
    valid = false;
  }
  if (ntn_cfg.distance_threshold > 1000) {
    fmt::print("Distance threshold must be in range [0, 1000].\n");
    valid = false;
  }
  if (ntn_cfg.epoch_time.sfn > 65535) {
    fmt::print("Epoch time SFN must be in range [0, 65535].\n");
    valid = false;
  }
  if (ntn_cfg.epoch_time.subframe_number > 9) {
    fmt::print("Epoch time subframe number must be in range [0, 9].\n");
    valid = false;
  }
  if (ntn_cfg.k_mac > 512) {
    fmt::print("K_MAC must be in range [0, 512].\n");
    valid = false;
  }
  if (ntn_cfg.ta_info.ta_common > 66485757) {
    fmt::print("TA common must be in range [0, 66485757].\n");
    valid = false;
  }
  if (std::abs(ntn_cfg.ta_info.ta_common_drift) > 257303) {
    fmt::print("TA common drift must be in range [-257303, 257303].\n");
    valid = false;
  }
  if (ntn_cfg.ta_info.ta_common_drift_variant > 28949) {
    fmt::print("TA common drift variant must be in range [0, 28949].\n");
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

  if (!validate_pdcch_appconfig(config)) {
    return false;
  }

  if (!validate_qos_appconfig(config.qos_cfg)) {
    return false;
  }

  if (!validate_expert_phy_appconfig(config.expert_phy_cfg)) {
    return false;
  }

  if (config.ntn_cfg.has_value()) {
    if (!validate_ntn_config(config.ntn_cfg.value())) {
      return false;
    }
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

    if (!validate_ru_ofh_appconfig(config)) {
      return false;
    }
  }

  if (!validate_test_mode_appconfig(config)) {
    return false;
  }

  return true;
}
