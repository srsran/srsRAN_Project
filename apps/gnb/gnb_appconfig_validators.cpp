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

#include "gnb_appconfig_validators.h"
#include "srsran/adt/span.h"
#include "srsran/adt/variant.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/phy/upper/channel_processors/prach_detector_phy_validator.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/prach/prach_helper.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/rlc/rlc_config.h"
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
static bool validate_ru_sdr_appconfig(const ru_sdr_appconfig& config, const cell_appconfig& cell_config)
{
  static constexpr phy_time_unit reference_time = phy_time_unit::from_units_of_kappa(16);

  if (!reference_time.is_sample_accurate(config.srate_MHz * 1e6)) {
    fmt::print("The sampling rate must be multiple of {:.2f} MHz.\n", 1e-6 / reference_time.to_seconds());
    return false;
  }

  // Validates the sampling rate is compatible with the PRACH sequence.
  subcarrier_spacing         common_scs = cell_config.cell.common_scs;
  prach_configuration        prach_info = prach_configuration_get(frequency_range::FR1,
                                                           band_helper::get_duplex_mode(cell_config.cell.band.value()),
                                                           cell_config.cell.prach_cfg.prach_config_index.value());
  prach_preamble_information preamble_info =
      is_long_preamble(prach_info.format)
          ? get_prach_preamble_long_info(prach_info.format)
          : get_prach_preamble_short_info(prach_info.format, to_ra_subcarrier_spacing(common_scs), false);
  if (!preamble_info.cp_length.is_sample_accurate(config.srate_MHz * 1e6)) {
    // List of common sampling rates for offering an alternative.
    static const std::array<double, 10> sampling_rates = {
        7.68, 11.52, 15.36, 23.04, 30.76, 46.08, 61.44, 92.16, 122.88, 184.32};
    std::vector<double> valid_sampling_rates;
    for (double sampling_rate : sampling_rates) {
      // A valid alternative sampling rate must be larger than the cell bandwidth and be sample accurate with the
      // preamble cyclic prefix.
      if ((sampling_rate > bs_channel_bandwidth_to_MHz(cell_config.cell.channel_bw_mhz)) &&
          (preamble_info.cp_length.is_sample_accurate(sampling_rate * 1e6))) {
        valid_sampling_rates.push_back(sampling_rate);
      }
    }

    fmt::print("PRACH Format {} with subcarrier spacing of {} is not compatible with {:.2f}MHz sampling rate. "
               "Valid sampling rates are {:,} MHz.\n",
               to_string(prach_info.format),
               to_string(common_scs),
               config.srate_MHz,
               span<const double>(valid_sampling_rates));
    return false;
  }

  static constexpr interval<float> lphy_dl_throttling_range(0.0F, 1.0F);
  if (!lphy_dl_throttling_range.contains(config.expert_cfg.lphy_dl_throttling)) {
    fmt::print("Low PHY throttling (i.e., {}) must be in range {}.\n",
               config.expert_cfg.lphy_dl_throttling,
               lphy_dl_throttling_range);
    return false;
  }

  if (config.expert_cfg.discontinuous_tx_mode && (config.device_driver == "zmq")) {
    fmt::print("Discontinuous transmission mode cannot be used with ZMQ.\n");
    return false;
  }

  static constexpr float subframe_duration_us = 1e3;
  float                  slot_duration_us =
      subframe_duration_us / static_cast<float>(get_nof_slots_per_subframe(cell_config.cell.common_scs));
  if (config.expert_cfg.discontinuous_tx_mode && (config.expert_cfg.power_ramping_time_us > 2 * slot_duration_us)) {
    fmt::print("Power ramping time, i.e., {:.1f} us, cannot exceed the duration of two NR slots, i.e., {:.1f} us.\n",
               config.expert_cfg.power_ramping_time_us,
               2 * slot_duration_us);
    return false;
  }

  if (config.expert_cfg.discontinuous_tx_mode && (config.expert_cfg.power_ramping_time_us < 0)) {
    fmt::print("Power ramping time, i.e., {:.1f} us, must be positive or zero.\n",
               config.expert_cfg.power_ramping_time_us);
    return false;
  }

  if (!validate_amplitude_control_appconfig(config.amplitude_cfg)) {
    return false;
  }

  return true;
}

/// Validates that the given ports are not duplicated. Returns true on success, otherwise false.
template <typename T>
__attribute_noinline__ static bool validate_duplicated_ports(span<const T> ports)
{
  std::vector<T> temp_ports(ports.begin(), ports.end());
  std::sort(temp_ports.begin(), temp_ports.end());

  return std::unique(temp_ports.begin(), temp_ports.end()) == temp_ports.end();
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

    if (cell_cfg.nof_antennas_ul > ofh_cell.ru_ul_port_id.size()) {
      fmt::print("RU number of uplink ports={} must be equal or greater than the number of reception antennas={}\n",
                 ofh_cell.ru_ul_port_id.size(),
                 cell_cfg.nof_antennas_ul);

      return false;
    }

    if (cell_cfg.prach_cfg.ports.size() > ofh_cell.ru_prach_port_id.size()) {
      fmt::print("RU number of PRACH ports={} must be equal or greater than the cell number of PRACH ports={}\n",
                 ofh_cell.ru_prach_port_id.size(),
                 cell_cfg.prach_cfg.ports.size());

      return false;
    }

    if (!validate_duplicated_ports<unsigned>(ofh_cell.ru_dl_port_id)) {
      fmt::print("Detected duplicated downlink port identifiers\n");

      return false;
    }

    if (!validate_duplicated_ports<unsigned>(ofh_cell.ru_ul_port_id)) {
      fmt::print("Detected duplicated uplink port identifiers\n");

      return false;
    }

    if (!validate_duplicated_ports<unsigned>(ofh_cell.ru_prach_port_id)) {
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

  if (config.max_rb_size < config.min_rb_size) {
    fmt::print("Invalid UE PDSCH RB range [{}, {}). The min_rb_size must be less or equal to the max_rb_size",
               config.min_rb_size,
               config.max_rb_size);
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

/// Validates the given PUCCH cell application configuration. Returns true on success, otherwise false.
static bool validate_pucch_cell_app_config(const base_cell_appconfig& config, subcarrier_spacing scs_common)
{
  const pucch_appconfig& pucch_cfg = config.pucch_cfg;
  if (not config.csi_cfg.csi_rs_enabled and pucch_cfg.nof_cell_csi_resources > 0) {
    fmt::print(
        "Number of PUCCH Format 2 cell resources for CSI must be zero when CSI-RS and CSI report are disabled.\n");
    return false;
  }

  if (config.csi_cfg.csi_rs_enabled and pucch_cfg.nof_cell_csi_resources == 0) {
    fmt::print("Number of PUCCH Format 2 cell resources for CSI must be greater than 0 when CSI-RS and CSI report are "
               "enabled.\n");
    return false;
  }

  const std::array<unsigned, 11> valid_sr_period_slots{1, 2, 4, 8, 10, 16, 20, 40, 80, 160, 320};
  const unsigned                 sr_period_slots = get_nof_slots_per_subframe(scs_common) * pucch_cfg.sr_period_msec;
  if (std::find(valid_sr_period_slots.begin(), valid_sr_period_slots.end(), sr_period_slots) ==
      valid_sr_period_slots.end()) {
    fmt::print("SR period of {}ms is not valid for {}kHz SCS.\n", pucch_cfg.sr_period_msec, scs_to_khz(scs_common));
    return false;
  }

  return true;
}

/// Validates the given PUCCH cell application configuration. Returns true on success, otherwise false.
static bool validate_ul_common_app_config(const ul_common_appconfig& config)
{
  if (config.max_ul_grants_per_slot <= config.max_pucchs_per_slot) {
    fmt::print("The max number of UL grants per slot should be greater than the maximum number of PUCCH grants.\n");
    return false;
  }

  return true;
}

/// Validates the given PRACH cell application configuration. Returns true on success, otherwise false.
static bool validate_prach_cell_app_config(const prach_appconfig& config, nr_band band, unsigned nof_rx_atennas)
{
  srsran_assert(config.prach_config_index.has_value(), "The PRACH configuration index must be set.");

  auto code =
      prach_helper::prach_config_index_is_valid(config.prach_config_index.value(), band_helper::get_duplex_mode(band));
  if (code.is_error()) {
    fmt::print("{}", code.error());
    return false;
  }

  code = prach_helper::zero_correlation_zone_is_valid(
      config.zero_correlation_zone, config.prach_config_index.value(), band_helper::get_duplex_mode(band));
  if (code.is_error()) {
    fmt::print("{}", code.error());
    return false;
  }

  if (config.ports.size() > nof_rx_atennas) {
    fmt::print("PRACH number of ports ({}) is bigger than the number of reception antennas ({}).\n",
               config.ports.size(),
               nof_rx_atennas);

    return false;
  }

  if (!validate_duplicated_ports<uint8_t>(config.ports)) {
    fmt::print("Detected duplicated PRACH port cell identifiers\n");
    return false;
  }

  for (auto port_id : config.ports) {
    if (port_id >= nof_rx_atennas) {
      fmt::print("PRACH port id '{}' out of range. Valid range {}-{}.\n", port_id, 0, nof_rx_atennas - 1);

      return false;
    }
  }

  return true;
}

static bool validate_phy_prach_configuration(const base_cell_appconfig& base_cell_config)
{
  const prach_appconfig& config = base_cell_config.prach_cfg;

  unsigned nof_rx_ports = config.ports.size();

  // Get PRACH info.
  subcarrier_spacing  common_scs = base_cell_config.common_scs;
  prach_configuration prach_info = prach_configuration_get(frequency_range::FR1,
                                                           band_helper::get_duplex_mode(base_cell_config.band.value()),
                                                           base_cell_config.prach_cfg.prach_config_index.value());

  // PRACH format type.
  prach_format_type format = prach_info.format;

  // Get preamble info.
  prach_preamble_information preamble_info =
      is_long_preamble(prach_info.format)
          ? get_prach_preamble_long_info(prach_info.format)
          : get_prach_preamble_short_info(prach_info.format, to_ra_subcarrier_spacing(common_scs), false);

  // PRACH subcarrier spacing.
  prach_subcarrier_spacing scs = preamble_info.scs;
  // Zero correlation zone.
  unsigned zero_correlation_zone = config.zero_correlation_zone;

  return validate_prach_detector_phy(format, scs, zero_correlation_zone, nof_rx_ports);
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

static bool validate_cell_sib_config(const base_cell_appconfig& cell_cfg)
{
  const sib_appconfig& sib_cfg = cell_cfg.sib_cfg;

  for (const auto& si_msg : sib_cfg.si_sched_info) {
    const unsigned si_period_slots =
        si_msg.si_period_rf * get_nof_slots_per_subframe(cell_cfg.common_scs) * NOF_SUBFRAMES_PER_FRAME;
    if (sib_cfg.si_window_len_slots > si_period_slots) {
      fmt::print("The SI window length in slots {} is larger than the SI message period {}.\n",
                 sib_cfg.si_window_len_slots,
                 si_period_slots);
      return false;
    }
  }

  // Check if there are repeated SIBs in the SI messages.
  std::vector<uint8_t> sibs_included;
  for (const auto& si_msg : sib_cfg.si_sched_info) {
    for (const uint8_t sib_it : si_msg.sib_mapping_info) {
      sibs_included.push_back(sib_it);
    }
  }
  std::sort(sibs_included.begin(), sibs_included.end());
  const auto duplicate_it = std::adjacent_find(sibs_included.begin(), sibs_included.end());
  if (duplicate_it != sibs_included.end()) {
    fmt::print("The SIB{} cannot be included more than once in the broadcast SI messages", *duplicate_it);
    return false;
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

  const auto ssb_scs =
      band_helper::get_most_suitable_ssb_scs(band_helper::get_band_from_dl_arfcn(config.dl_arfcn), config.common_scs);
  if (ssb_scs != config.common_scs) {
    fmt::print("Common SCS {}kHz is not equal to SSB SCS {}kHz. Different SCS for common and SSB is not supported.\n",
               scs_to_khz(config.common_scs),
               scs_to_khz(ssb_scs));
    return false;
  }

  if (!validate_pdsch_cell_app_config(config.pdsch_cfg)) {
    return false;
  }

  if (!validate_pucch_cell_app_config(config, config.common_scs)) {
    return false;
  }

  if (!validate_ul_common_app_config(config.ul_common_cfg)) {
    return false;
  }

  if (!validate_pusch_cell_app_config(config.pusch_cfg)) {
    return false;
  }

  nr_band band = config.band.has_value() ? config.band.value() : band_helper::get_band_from_dl_arfcn(config.dl_arfcn);
  if (!validate_prach_cell_app_config(config.prach_cfg, band, config.nof_antennas_ul)) {
    return false;
  }

  if (!validate_phy_prach_configuration(config)) {
    return false;
  }

  if (band_helper::get_duplex_mode(band) == duplex_mode::TDD and config.tdd_ul_dl_cfg.has_value() and
      !validate_tdd_ul_dl_appconfig(config.tdd_ul_dl_cfg.value(), config.common_scs)) {
    return false;
  }

  if (!validate_cell_sib_config(config)) {
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
  unsigned tac = config[0].cell.tac;
  for (const auto& cell : config) {
    if (!validate_cells_appconfig(cell)) {
      return false;
    }
    if (cell.cell.tac != tac) {
      fmt::print("The TAC must be the same for all cells\n");
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

/// Validates the given CU-CP configuration. Returns true on success, otherwise false.
static bool validate_cu_cp_appconfig(const cu_cp_appconfig& config, const sib_appconfig& sib_cfg)
{
  // only check if the ue_context_setup_timout is larger than T310
  if (config.ue_context_setup_timeout_s * 1000 < sib_cfg.ue_timers_and_constants.t310) {
    fmt::print("ue_context_setup_timeout_s ({}ms) must be larger than T310 ({}ms)\n",
               config.ue_context_setup_timeout_s * 1000,
               sib_cfg.ue_timers_and_constants.t310);
    return false;
  }
  return true;
}

/// Validates the given PDCP configuration. Returns true on success, otherwise false.
static bool validate_pdcp_appconfig(five_qi_t five_qi, const pdcp_appconfig& config)
{
  if (config.integrity_protection_required) {
    fmt::print("PDCP DRB integrity protection is not supported yet. {}\n", five_qi);
    return false;
  }

  // Check TX
  if (config.tx.sn_field_length != 12 && config.tx.sn_field_length != 18) {
    fmt::print("PDCP TX SN length is neither 12 or 18 bits. {} SN={}\n", five_qi, config.tx.sn_field_length);
    return false;
  }
  if (config.tx.status_report_required) {
    fmt::print("PDCP TX status report required not supported yet. {}\n", five_qi);
    return false;
  }

  // Check RX
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
    srslog::basic_logger& logger = srslog::fetch_basic_logger("GNB");
    fmt::print("PDCP t-Reordering=infinity on DRBs is not advised. It can cause data stalls. {}\n", five_qi);
    logger.warning("PDCP t-Reordering=infinity on DRBs is not advised. It can cause data stalls. {}", five_qi);
  }

  if (config.rx.out_of_order_delivery) {
    fmt::print("PDCP RX out-of-order delivery is not supported. {}\n", five_qi);
    return false;
  }
  return true;
}

static bool validate_rlc_um_appconfig(five_qi_t five_qi, const rlc_um_appconfig& config)
{
  // Validate TX
  rlc_um_sn_size   tmp_sn_size;
  rlc_t_reassembly tmp_t_reassembly;
  if (!from_number(tmp_sn_size, config.tx.sn_field_length)) {
    fmt::print("RLC UM TX SN length is neither 6 or 12 bits. {} sn_size={}\n", five_qi, config.tx.sn_field_length);
    return false;
  }
  if (config.tx.queue_size == 0) {
    fmt::print("RLC TX queue size cannot be 0. {}\n", five_qi);
    return false;
  }
  // Validate RX
  if (!from_number(tmp_sn_size, config.rx.sn_field_length)) {
    fmt::print("RLC TX queue size cannot be 0. {}\n", five_qi);
    return false;
  }
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
static bool validate_rlc_am_appconfig(id_type id, const rlc_am_appconfig& config)
{
  // Validate TX
  rlc_am_sn_size         tmp_sn_size;
  rlc_max_retx_threshold tmp_max_retx_threshold;
  rlc_t_poll_retransmit  tmp_t_poll_retransmit;
  rlc_t_reassembly       tmp_t_reassembly;
  rlc_poll_pdu           tmp_poll_pdu;
  rlc_poll_kilo_bytes    tmp_poll_bytes;
  rlc_t_status_prohibit  tmp_t_status_prohibit;

  if (!from_number(tmp_sn_size, config.tx.sn_field_length)) {
    fmt::print("RLC AM TX SN length is neither 12 or 18 bits. {} sn_size={}\n", id, config.tx.sn_field_length);
    return false;
  }

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

  if (!rlc_max_retx_threshold_from_int(tmp_max_retx_threshold, config.tx.max_retx_thresh)) {
    fmt::print("Invalid RLC AM TX max retx threshold. {} max_retx_threshold={}\n", id, config.tx.max_retx_thresh);
    fmt::print(" Valid values are: t1, t2, t3, t4, t6, t8, t16, t32\n");
    return false;
  }

  if (!rlc_poll_pdu_from_int(tmp_poll_pdu, config.tx.poll_pdu)) {
    fmt::print("Invalid RLC AM TX PollPDU. {} poll_pdu={}\n", id, config.tx.poll_pdu);
    fmt::print(" Valid values are:"
               "p4, p8, p16, p32, p64, p128, p256, p512, p1024, p2048,"
               " p4096, p6144, p8192, p12288, p16384,p20480,"
               " p24576, p28672, p32768, p40960, p49152, p57344, p65536\n");
    return false;
  }

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

  // Validate RX
  if (!from_number(tmp_sn_size, config.rx.sn_field_length)) {
    fmt::print("RLC AM RX SN length is neither 12 or 18 bits. {} sn_size={}\n", id, config.rx.sn_field_length);
    return false;
  }
  if (!rlc_t_reassembly_from_int(tmp_t_reassembly, config.rx.t_reassembly)) {
    fmt::print("RLC AM RX t-Reassembly is invalid. {} t_reassembly={}\n", id, config.rx.t_reassembly);
    fmt::print("Valid values are:"
               " ms40, ms45, ms50, ms55, ms60, ms65, ms70,"
               " ms75, ms80, ms85, ms90, ms95, ms100, ms110,"
               " ms120, ms130, ms140, ms150, ms160, ms170,"
               " ms180, ms190, ms200\n");
    return false;
  }
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

static bool validate_rlc_appconfig(five_qi_t five_qi, const rlc_appconfig& config)
{
  // Check mode
  srslog::basic_logger& logger = srslog::fetch_basic_logger("GNB");
  if (config.mode != "am" && config.mode != "um-bidir") {
    fmt::print("RLC mode is neither \"am\" or \"um-bidir\". {} mode={}\n", five_qi, config.mode);
    logger.warning("RLC mode is neither \"am\" or \"um-bidir\". {} mode={}", five_qi, config.mode);
    return false;
  }

  // Check AM
  if (config.mode == "am" && !validate_rlc_am_appconfig(five_qi, config.am)) {
    fmt::print("RLC AM config is invalid. {}\n", five_qi);
    logger.warning("RLC AM config is invalid. {}\n", five_qi);
    return false;
  }

  // Check UM
  if (config.mode == "um-bidir" && !validate_rlc_um_appconfig(five_qi, config.um)) {
    fmt::print("RLC UM config is invalid. {}\n", five_qi);
    logger.warning("RLC UM config is invalid. {}\n", five_qi);
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
    if (!validate_rlc_appconfig(qos.five_qi, qos.rlc)) {
      return false;
    }
  }
  return true;
}

/// Validates the given SRB configuration. Returns true on success, otherwise false.
static bool validate_srb_appconfig(const std::map<srb_id_t, srb_appconfig>& config)
{
  for (const auto& srb : config) {
    if (srb.first != srb_id_t::srb1 && srb.first != srb_id_t::srb2 && srb.first != srb_id_t::srb3) {
      fmt::print("Cannot configure SRB{}. Only SRB1, SRB2 and SRB3 can be configured", srb.first);
      return false;
    }
    if (!validate_rlc_am_appconfig(srb.first, srb.second.rlc)) {
      return false;
    }
  }
  return true;
}

/// Validates the given security configuration. Returns true on success, otherwise false.
static bool validate_security_appconfig(const security_appconfig& config)
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
  bool valid = true;

  if ((config.pusch_sinr_calc_method != "channel_estimator") &&
      (config.pusch_sinr_calc_method != "post_equalization") && (config.pusch_sinr_calc_method != "evm")) {
    fmt::print(
        "Invalid PUSCH SINR calculation method. Valid types are: channel_estimator, post_equalization and evm.\n");
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
      const auto     ssb_coreset0_freq_loc = band_helper::get_ssb_coreset0_freq_location_for_cset0_idx(
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
      // been set to 3. And, we use only pos2 or pos1.
      const pdcch_type0_css_coreset_description desc = srsran::pdcch_type0_css_coreset_get(
          band, base_cell.common_scs, base_cell.common_scs, cs0_idx, ssb_coreset0_freq_loc->k_ssb.to_uint());
      if (desc.pattern != PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern and desc.nof_symb_coreset == 3) {
        fmt::print("CORESET duration of 3 OFDM symbols corresponding to CORESET#0 index={} is not supported\n",
                   cs0_idx);
        return false;
      }
      if (base_cell.pdcch_cfg.common.max_coreset0_duration.has_value() and
          desc.pattern != PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern and
          desc.nof_symb_coreset > base_cell.pdcch_cfg.common.max_coreset0_duration.value()) {
        fmt::print("Configured CORESET#0 index={} results in duration={} > maximum CORESET#0 duration configured={}. "
                   "Try increasing maximum CORESET#0 duration or pick another CORESET#0 index\n",
                   cs0_idx,
                   desc.nof_symb_coreset,
                   base_cell.pdcch_cfg.common.max_coreset0_duration.value());
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
    if (base_cell.pdcch_cfg.dedicated.ss2_type == search_space_configuration::type_t::common and
        base_cell.pdcch_cfg.dedicated.dci_format_0_1_and_1_1) {
      fmt::print("Non-fallback DCI format not allowed in Common SearchSpace\n");
      return false;
    }
  }
  return true;
}

static bool validate_test_mode_appconfig(const gnb_appconfig& config)
{
  if ((config.test_mode_cfg.test_ue.ri > 1) and
      not config.cells_cfg.front().cell.pdcch_cfg.dedicated.dci_format_0_1_and_1_1) {
    fmt::print("For test mode, RI shall not be set if UE is configured to use DCI format 1_0\n");
    return false;
  }
  if (config.test_mode_cfg.test_ue.ri > config.cells_cfg.front().cell.nof_antennas_dl) {
    fmt::print("For test mode, RI cannot be higher than the number of DL antenna ports ({} > {})\n",
               config.test_mode_cfg.test_ue.ri,
               config.cells_cfg.front().cell.nof_antennas_dl);
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

static bool validate_ntn_config(const ntn_config& ntn_cfg)
{
  bool valid = true;

  if (ntn_cfg.cell_specific_koffset > 1023) {
    fmt::print("Cell specific koffset must be in range [0, 1023].\n");
    valid = false;
  }
  if (ntn_cfg.distance_threshold.has_value()) {
    if (ntn_cfg.distance_threshold.value() > 1000) {
      fmt::print("Distance threshold must be in range [0, 1000].\n");
      valid = false;
    }
  }
  if (ntn_cfg.epoch_time.has_value()) {
    if (ntn_cfg.epoch_time.value().sfn > 65535) {
      fmt::print("Epoch time SFN must be in range [0, 65535].\n");
      valid = false;
    }
    if (ntn_cfg.epoch_time.value().subframe_number > 9) {
      fmt::print("Epoch time subframe number must be in range [0, 9].\n");
      valid = false;
    }
  }
  if (ntn_cfg.k_mac.has_value()) {
    if (ntn_cfg.k_mac.value() > 512) {
      fmt::print("K_MAC must be in range [0, 512].\n");
      valid = false;
    }
  }
  if (ntn_cfg.ta_info.has_value()) {
    if (ntn_cfg.ta_info.value().ta_common > 66485757) {
      fmt::print("TA common must be in range [0, 66485757].\n");
      valid = false;
    }
    if (std::abs(ntn_cfg.ta_info.value().ta_common_drift) > 257303) {
      fmt::print("TA common drift must be in range [-257303, 257303].\n");
      valid = false;
    }
    if (ntn_cfg.ta_info.value().ta_common_drift_variant > 28949) {
      fmt::print("TA common drift variant must be in range [0, 28949].\n");
      valid = false;
    }
  }
  return valid;
}

static bool validate_hal_config(const optional<hal_appconfig>& config)
{
#ifdef DPDK_FOUND
  if (config && config->eal_args.empty()) {
    fmt::print("It is mandatory to fill the EAL configuration arguments to initialize DPDK correctly\n");
    return false;
  }
#else
  if (config) {
    fmt::print("Unable to use DPDK as the application was not compiled with DPDK support\n");
    return false;
  }
#endif
  return true;
}

static bool validate_upper_phy_threads_appconfig(const upper_phy_threads_appconfig& config,
                                                 unsigned                           max_processing_delay_slots)
{
  static const interval<unsigned, true> nof_ul_dl_threads_range(1, std::thread::hardware_concurrency());
  static const interval<unsigned, true> nof_pdsch_threads_range(2, std::thread::hardware_concurrency());

  bool valid = true;

  if (!nof_ul_dl_threads_range.contains(config.nof_ul_threads)) {
    fmt::print(
        "Number of PHY UL threads (i.e., {}) must be in range {}.\n", config.nof_ul_threads, nof_ul_dl_threads_range);
    valid = false;
  }

  if ((config.pdsch_processor_type != "auto") && (config.pdsch_processor_type != "concurrent") &&
      config.pdsch_processor_type != "generic" && (config.pdsch_processor_type != "lite")) {
    fmt::print("Invalid PDSCH processor type. Valid types are: auto, generic, concurrent and lite.\n");
    valid = false;
  }

  if ((config.pdsch_processor_type == "concurrent") && !nof_pdsch_threads_range.contains(config.nof_dl_threads)) {
    fmt::print("For concurrent PDSCH processor. Number of PHY DL threads (i.e., {}) must be in range {}.\n",
               config.nof_dl_threads,
               nof_pdsch_threads_range);
    valid = false;
  }

  if (!nof_ul_dl_threads_range.contains(config.nof_dl_threads)) {
    fmt::print(
        "Number of PHY DL threads (i.e., {}) must be in range {}.\n", config.nof_dl_threads, nof_ul_dl_threads_range);
    valid = false;
  }

  return valid;
}

static bool validate_expert_execution_appconfig(const gnb_appconfig& config)
{
  if (!validate_upper_phy_threads_appconfig(config.expert_execution_cfg.threads.upper_threads,
                                            config.expert_phy_cfg.max_processing_delay_slots)) {
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

  if (!validate_cu_cp_appconfig(config.cu_cp_cfg, config.cells_cfg.front().cell.sib_cfg)) {
    return false;
  }

  if (!validate_cells_appconfig(config.cells_cfg)) {
    return false;
  }

  if (!config.log_cfg.phy_rx_symbols_filename.empty() && config.log_cfg.phy_rx_symbols_port.has_value() &&
      (config.log_cfg.phy_rx_symbols_port.value() >= config.cells_cfg.front().cell.nof_antennas_ul)) {
    fmt::print("Requested IQ dump from Rx port {}, valid Rx ports are 0-{}.\n",
               config.log_cfg.phy_rx_symbols_port.value(),
               config.cells_cfg.front().cell.nof_antennas_ul - 1);
    return false;
  }

  if (!validate_pdcch_appconfig(config)) {
    return false;
  }

  if (!validate_qos_appconfig(config.qos_cfg)) {
    return false;
  }

  if (!validate_srb_appconfig(config.srb_cfg)) {
    return false;
  }

  if (!validate_security_appconfig(config.cu_cp_cfg.security_config)) {
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

    if (!validate_ru_sdr_appconfig(sdr_cfg, config.cells_cfg.front())) {
      return false;
    }

    if (sdr_cfg.srate_MHz < bs_channel_bandwidth_to_MHz(config.cells_cfg.front().cell.channel_bw_mhz)) {
      fmt::print("Sampling rate (i.e. {} MHz) is too low for the requested channel bandwidth (i.e. {} MHz).\n",
                 sdr_cfg.srate_MHz,
                 bs_channel_bandwidth_to_MHz(config.cells_cfg.front().cell.channel_bw_mhz));
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

  if (!validate_hal_config(config.hal_config)) {
    return false;
  }

  if (config.hal_config && config.cells_cfg.size() > 1) {
    fmt::print("As a temporary limitation, DPDK can only be used with a single cell\n");
    return false;
  }

  if (!validate_expert_execution_appconfig(config)) {
    return false;
  }

  return true;
}
