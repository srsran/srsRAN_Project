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

#include "du_high_config_validator.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/nr_cell_identity.h"
#include "srsran/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsran/ran/prach/prach_helper.h"
#include "srsran/rlc/rlc_config.h"

using namespace srsran;

static bool validate_pcap_configs(const du_high_unit_config& config)
{
  if (config.pcaps.mac.enabled) {
    for (const auto& cell : config.cells_cfg) {
      // FIXME don't use just the nof_antennas, but compute MAX PDU size from bandwidth, MCS table, etc.
      if (cell.cell.nof_antennas_dl >= 2 && config.pcaps.mac.type != "dlt") {
        fmt::print("Using more than two DL antennas might make the MAC PDU larger than what is supported in the UDP "
                   "wrapper. Use PCAP DLT type to avoid pcap corruption.\n");
        return false;
      }
      if (cell.cell.nof_antennas_ul >= 2 && config.pcaps.mac.type != "dlt") {
        fmt::print("Using more than two UL antennas might make the MAC PDU larger than what is supported in the UDP "
                   "wrapper. Use PCAP DLT type to avoid pcap corruption.\n");
        return false;
      }
    }
  }
  return true;
}

static bool validate_expert_execution_unit_config(const du_high_unit_config&       config,
                                                  const os_sched_affinity_bitmask& available_cpus)
{
  // Configure more cells for expert execution than the number of cells is an error.
  if (config.expert_execution_cfg.cell_affinities.size() != config.cells_cfg.size()) {
    fmt::print(
        "Using different number of cells for DU high expert execution '{}' than the number of defined cells '{}'\n",
        config.expert_execution_cfg.cell_affinities.size(),
        config.cells_cfg.size());

    return false;
  }

  auto validate_cpu_range = [](const os_sched_affinity_bitmask& allowed_cpus_mask,
                               const os_sched_affinity_bitmask& mask,
                               const std::string&               name) {
    auto invalid_cpu_ids = mask.subtract(allowed_cpus_mask);
    if (not invalid_cpu_ids.empty()) {
      fmt::print(
          "CPU cores {} selected in '{}' option doesn't belong to the available cpuset.\n", invalid_cpu_ids, name);
      return false;
    }

    return true;
  };

  for (const auto& cell : config.expert_execution_cfg.cell_affinities) {
    if (!validate_cpu_range(available_cpus, cell.l2_cell_cpu_cfg.mask, "l2_cell_cpus")) {
      return false;
    }
  }

  return true;
}

template <typename id_type>
static bool validate_rlc_am_unit_config(id_type id, const du_high_unit_rlc_am_config& config)
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

/// Validates the given SRB configuration. Returns true on success, otherwise false.
static bool validate_srb_unit_config(const std::map<srb_id_t, du_high_unit_srb_config>& config)
{
  for (const auto& srb : config) {
    if (srb.first != srb_id_t::srb1 && srb.first != srb_id_t::srb2 && srb.first != srb_id_t::srb3) {
      fmt::print("Cannot configure SRB{}. Only SRB1, SRB2 and SRB3 can be configured", srb.first);
      return false;
    }
    if (!validate_rlc_am_unit_config(srb.first, srb.second.rlc)) {
      return false;
    }
  }
  return true;
}

static bool validate_pdcch_unit_config(const du_high_unit_base_cell_config base_cell)
{
  const auto band =
      base_cell.band.has_value() ? *base_cell.band : band_helper::get_band_from_dl_arfcn(base_cell.dl_arfcn);
  const unsigned nof_crbs =
      band_helper::get_n_rbs_from_bw(base_cell.channel_bw_mhz, base_cell.common_scs, band_helper::get_freq_range(band));
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
      fmt::print("CORESET duration of 3 OFDM symbols corresponding to CORESET#0 index={} is not supported\n", cs0_idx);
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
static bool validate_pdsch_cell_unit_config(const du_high_unit_pdsch_config& config, unsigned cell_bw_crbs)
{
  if (config.min_ue_mcs > config.max_ue_mcs) {
    fmt::print("Invalid UE MCS range (i.e., [{}, {}]). The min UE MCS must be less than or equal to the max UE MCS.\n",
               config.min_ue_mcs,
               config.max_ue_mcs);
    return false;
  }

  unsigned max_ue_mcs = 28;
  if (config.mcs_table == pdsch_mcs_table::qam256) {
    max_ue_mcs = 27;
  }

  if (config.min_ue_mcs > max_ue_mcs) {
    fmt::print("Invalid PDSCH min_ue_mcs (i.e., {}) for the selected MCS table (i.e., {}).\n",
               config.min_ue_mcs,
               (config.mcs_table == pdsch_mcs_table::qam256) ? "qam256" : "qam64");
    return false;
  }

  if (not validate_rv_sequence(config.rv_sequence)) {
    return false;
  }

  if (config.end_rb <= config.start_rb) {
    fmt::print("Invalid RB allocation range [{}, {}) for UE PDSCHs. The start_rb must be less or equal to the end_rb",
               config.start_rb,
               config.end_rb);
    return false;
  }

  if (config.max_rb_size <= config.min_rb_size) {
    fmt::print("Invalid UE PDSCH RB range [{}, {}). The min_rb_size must be less or equal to the max_rb_size",
               config.min_rb_size,
               config.max_rb_size);
    return false;
  }

  if (config.end_rb < config.start_rb) {
    fmt::print("Invalid RB allocation range [{}, {}) for UE PDSCHs. The start_rb must be less or equal to the end_rb",
               config.start_rb,
               config.end_rb);
    return false;
  }

  if (config.start_rb >= cell_bw_crbs) {
    fmt::print("Invalid start RB {} for UE PDSCHs. The start_rb must be less than the cell BW", config.start_rb);
    return false;
  }

  return true;
}

/// Validates the given PUSCH cell application configuration. Returns true on success, otherwise false.
static bool validate_pusch_cell_unit_config(const du_high_unit_pusch_config& config, unsigned cell_crbs)
{
  if (config.min_ue_mcs > config.max_ue_mcs) {
    fmt::print("Invalid UE MCS range (i.e., [{}, {}]). The min UE MCS must be less than or equal to the max UE MCS.\n",
               config.min_ue_mcs,
               config.max_ue_mcs);
    return false;
  }

  unsigned max_ue_mcs = 28;
  if (config.mcs_table == pusch_mcs_table::qam256) {
    max_ue_mcs = 27;
  }

  if (config.min_ue_mcs > max_ue_mcs) {
    fmt::print("Invalid PUSCH min_ue_mcs (i.e., {}) for the selected MCS table (i.e., {}).\n",
               config.min_ue_mcs,
               (config.mcs_table == pusch_mcs_table::qam256) ? "qam256" : "qam64");
    return false;
  }

  if (not validate_rv_sequence(config.rv_sequence)) {
    return false;
  }

  if (config.max_rb_size < config.min_rb_size) {
    fmt::print("Invalid UE PUSCH RB range [{}, {}). The min_rb_size must be less or equal to the max_rb_size",
               config.min_rb_size,
               config.max_rb_size);
    return false;
  }

  if (config.end_rb < config.start_rb) {
    fmt::print("Invalid RB allocation range [{}, {}) for UE PUSCHs. The start_rb must be less or equal to the end_rb",
               config.start_rb,
               config.end_rb);
    return false;
  }

  if (config.start_rb >= cell_crbs) {
    fmt::print("Invalid start RB {} for UE PUSCHs. The start_rb must be less than the cell BW", config.start_rb);
    return false;
  }

  return true;
}

/// Validates the given PUCCH cell application configuration. Returns true on success, otherwise false.
static bool validate_pucch_cell_unit_config(const du_high_unit_base_cell_config& config, subcarrier_spacing scs_common)
{
  const du_high_unit_pucch_config& pucch_cfg = config.pucch_cfg;
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

  static constexpr std::array<unsigned, 12> valid_sr_period_slots{1, 2, 4, 5, 8, 10, 16, 20, 40, 80, 160, 320};
  const auto sr_period_slots = static_cast<unsigned>(get_nof_slots_per_subframe(scs_common) * pucch_cfg.sr_period_msec);

  // Check that the SR period in milliseconds leads to an integer number of slots.
  if (get_nof_slots_per_subframe(scs_common) * pucch_cfg.sr_period_msec != static_cast<float>(sr_period_slots)) {
    fmt::print("SR period (i.e., {}ms) times the number of slots per subframe (i.e., {}) must be an integer number of "
               "slots.\n",
               pucch_cfg.sr_period_msec,
               get_nof_slots_per_subframe(scs_common));
    return false;
  }

  if (std::find(valid_sr_period_slots.begin(), valid_sr_period_slots.end(), sr_period_slots) ==
      valid_sr_period_slots.end()) {
    fmt::print("SR period of {}ms is not valid for {}kHz SCS.\n", pucch_cfg.sr_period_msec, scs_to_khz(scs_common));
    return false;
  }

  return true;
}

/// Validates the given PUCCH cell application configuration. Returns true on success, otherwise false.
static bool validate_ul_common_unit_config(const du_high_unit_ul_common_config& config)
{
  if (config.max_ul_grants_per_slot <= config.max_pucchs_per_slot) {
    fmt::print("The max number of UL grants per slot should be greater than the maximum number of PUCCH grants.\n");
    return false;
  }

  return true;
}

/// Validates that the given ports are not duplicated. Returns true on success, otherwise false.
template <typename T>
[[gnu::noinline]] static bool validate_duplicated_ports(span<const T> ports)
{
  std::vector<T> temp_ports(ports.begin(), ports.end());
  std::sort(temp_ports.begin(), temp_ports.end());

  return std::unique(temp_ports.begin(), temp_ports.end()) == temp_ports.end();
}

/// Validates the given PRACH cell application configuration. Returns true on success, otherwise false.
static bool
validate_prach_cell_unit_config(const du_high_unit_prach_config& config, nr_band band, unsigned nof_rx_atennas)
{
  srsran_assert(config.prach_config_index.has_value(), "The PRACH configuration index must be set.");

  auto code =
      prach_helper::prach_config_index_is_valid(config.prach_config_index.value(), band_helper::get_duplex_mode(band));
  if (not code.has_value()) {
    fmt::print("{}", code.error());
    return false;
  }

  code = prach_helper::zero_correlation_zone_is_valid(
      config.zero_correlation_zone, config.prach_config_index.value(), band_helper::get_duplex_mode(band));
  if (not code.has_value()) {
    fmt::print("{}", code.error());
    return false;
  }

  code = prach_helper::nof_ssb_per_ro_and_nof_cb_preambles_per_ssb_is_valid(config.nof_ssb_per_ro,
                                                                            config.nof_cb_preambles_per_ssb);
  if (not code.has_value()) {
    fmt::print("{}", code.error());
    return false;
  }

  // See TS 38.331, ssb-perRACH-OccasionAndCB-PreamblesPerSSB and totalNumberOfRA-Preambles.
  // totalNumberOfRA-Preambles should be a multiple of the number of SSBs per RACH occasion.
  if (config.total_nof_ra_preambles.has_value()) {
    bool is_total_nof_ra_preambles_valid = true;
    if (config.nof_ssb_per_ro >= 1) {
      if (config.total_nof_ra_preambles.value() % static_cast<uint8_t>(config.nof_ssb_per_ro) != 0) {
        is_total_nof_ra_preambles_valid = false;
      }
      // Ensure \c config.total_nof_ra_preambles can accommodate contention based RA preambles.
      // NOTE: \c config.total_nof_ra_preambles nof. RA preambles are shared among \c config.nof_ssb_per_ro nof. SSB
      // beams.
      if ((config.nof_cb_preambles_per_ssb * config.nof_ssb_per_ro) > config.total_nof_ra_preambles.value()) {
        is_total_nof_ra_preambles_valid = false;
      }
    } else {
      // Number of SSBs per RACH occasion is 1/8 or 1/4 or 1/2.
      const auto product = config.total_nof_ra_preambles.value() * config.nof_ssb_per_ro;
      if ((product - static_cast<uint8_t>(product)) > 0) {
        is_total_nof_ra_preambles_valid = false;
      }
      // Ensure \c config.total_nof_ra_preambles can accommodate contention based RA preambles.
      // NOTE: Each SSB beam has multiple RACH occasions and each occasion has \c config.total_nof_ra_preambles RA
      // preambles.
      if (config.nof_cb_preambles_per_ssb > config.total_nof_ra_preambles.value()) {
        is_total_nof_ra_preambles_valid = false;
      }
    }

    if (not is_total_nof_ra_preambles_valid) {
      fmt::print("Total nof. RA preambles ({}) should be a multiple of the number of SSBs per RACH occasion ({}).\n",
                 config.total_nof_ra_preambles.value(),
                 config.nof_ssb_per_ro);
      return false;
    }
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

static bool validate_tdd_ul_dl_pattern_unit_config(const tdd_ul_dl_pattern_unit_config& config,
                                                   subcarrier_spacing                   common_scs)
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
static bool validate_tdd_ul_dl_unit_config(const du_high_unit_tdd_ul_dl_config& config, subcarrier_spacing common_scs)
{
  if (not validate_tdd_ul_dl_pattern_unit_config(config.pattern1, common_scs)) {
    return false;
  }
  if (config.pattern2.has_value() and not validate_tdd_ul_dl_pattern_unit_config(config.pattern2.value(), common_scs)) {
    return false;
  }

  const unsigned period_msec =
      (config.pattern1.dl_ul_period_slots + (config.pattern2.has_value() ? config.pattern2->dl_ul_period_slots : 0)) /
      get_nof_slots_per_subframe(common_scs);
  // As per TS 38.213, clause 11.1, "A UE expects that P + P2 divides 20 msec".
  if (20 % period_msec != 0) {
    fmt::print("Invalid TDD pattern total periodicity={}ms. It must divide 20 msec.\n", period_msec);
    return false;
  }

  return true;
}

static bool validate_dl_arfcn_and_band(const du_high_unit_base_cell_config& config)
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
    if (not ret.has_value()) {
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

static bool validate_cell_sib_config(const du_high_unit_base_cell_config& cell_cfg)
{
  // See TS 38.331, V17.0.0, \c type1-r17 in \c SIB-TypeInfo-v1700.
  static const unsigned r17_min_sib_type = 15;

  const du_high_unit_sib_config& sib_cfg = cell_cfg.sib_cfg;

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

  std::vector<uint8_t>  sibs_included;
  std::vector<unsigned> si_window_positions;
  for (const auto& si_msg : sib_cfg.si_sched_info) {
    for (const uint8_t sib_it : si_msg.sib_mapping_info) {
      // si-WindowPosition-r17 is part of release 17 specification only. See TS 38.331, V17.0.0, \c SchedulingInfo2-r17.
      if (sib_it < r17_min_sib_type and si_msg.si_window_position.has_value()) {
        fmt::print("The SIB{} cannot be configured with SI-window position", sib_it);
        return false;
      }
      sibs_included.push_back(sib_it);
    }
    if (si_msg.si_window_position.has_value()) {
      si_window_positions.push_back(si_msg.si_window_position.value());
    }
  }
  std::sort(sibs_included.begin(), sibs_included.end());
  // Check if there are repeated SIBs in the SI messages.
  const auto duplicate_it = std::adjacent_find(sibs_included.begin(), sibs_included.end());
  if (duplicate_it != sibs_included.end()) {
    fmt::print("The SIB{} cannot be included more than once in the broadcast SI messages", *duplicate_it);
    return false;
  }

  // Check whether SI window position when provided in SI scheduling information is in ascending order. See TS 38.331,
  // \c si-WindowPosition.
  for (unsigned i = 0, j = 0; i < si_window_positions.size() && j < si_window_positions.size(); ++i, ++j) {
    if (si_window_positions[i] > si_window_positions[j]) {
      fmt::print("The SI window position in the subsequent entry in SI scheduling information must have higher value "
                 "than in the previous entry ({}>{})",
                 si_window_positions[i],
                 si_window_positions[j]);
      return false;
    }
  }

  return true;
}

/// Validates the given cell application configuration. Returns true on success, otherwise false.
static bool validate_base_cell_unit_config(const du_high_unit_base_cell_config& config)
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
  const nr_band band =
      config.band.has_value() ? config.band.value() : band_helper::get_band_from_dl_arfcn(config.dl_arfcn);
  const unsigned nof_crbs =
      band_helper::get_n_rbs_from_bw(config.channel_bw_mhz, config.common_scs, band_helper::get_freq_range(band));

  if (!validate_pdsch_cell_unit_config(config.pdsch_cfg, nof_crbs)) {
    return false;
  }

  if (!validate_pdcch_unit_config(config)) {
    return false;
  }

  if (!validate_pucch_cell_unit_config(config, config.common_scs)) {
    return false;
  }

  if (!validate_ul_common_unit_config(config.ul_common_cfg)) {
    return false;
  }

  if (!validate_pusch_cell_unit_config(config.pusch_cfg, nof_crbs)) {
    return false;
  }

  if (!validate_prach_cell_unit_config(config.prach_cfg, band, config.nof_antennas_ul)) {
    return false;
  }

  if (band_helper::get_duplex_mode(band) == duplex_mode::TDD and config.tdd_ul_dl_cfg.has_value() and
      !validate_tdd_ul_dl_unit_config(config.tdd_ul_dl_cfg.value(), config.common_scs)) {
    return false;
  }

  if (!validate_cell_sib_config(config)) {
    return false;
  }

  return true;
}

static bool validate_cell_unit_config(const du_high_unit_cell_config& config)
{
  return validate_base_cell_unit_config(config.cell);
}

/// Validates the given list of cell application configuration. Returns true on success, otherwise false.
static bool validate_cells_unit_config(span<const du_high_unit_cell_config> config, const gnb_id_t& gnb_id)
{
  unsigned tac = config[0].cell.tac;
  for (const auto& cell : config) {
    if (!validate_cell_unit_config(cell)) {
      return false;
    }
    if (cell.cell.tac != tac) {
      fmt::print("The TAC must be the same for all cells\n");
      return false;
    }

    if (cell.cell.sector_id.has_value() and
        not nr_cell_identity::create(gnb_id, cell.cell.sector_id.value()).has_value()) {
      fmt::print("Invalid Sector ID {}, for a gNB Id of {} bits\n", cell.cell.sector_id.value(), gnb_id.bit_length);
      return false;
    }
  }

  // Checks parameter collisions between cells that can lead to problems.
  for (const auto* it = config.begin(); it != config.end(); ++it) {
    for (const auto* it2 = it + 1; it2 != config.end(); ++it2) {
      const auto& cell1 = *it;
      const auto& cell2 = *it2;

      // Check if both cells are on the same frequency.
      if (cell1.cell.dl_arfcn == cell2.cell.dl_arfcn) {
        // Two cells on the same frequency should not have the same physical cell identifier.
        if (cell1.cell.pci == cell2.cell.pci) {
          fmt::print("Warning: two cells with the same DL ARFCN (i.e., {}) have the same PCI (i.e., {}).\n",
                     cell1.cell.dl_arfcn,
                     cell1.cell.pci);
        }
        if (cell1.cell.sector_id.has_value() and cell1.cell.sector_id == cell2.cell.sector_id and
            cell1.cell.plmn == cell2.cell.plmn) {
          fmt::print("Error: two cells with the same PLMN (i.e., {}) and cell ID (i.e., {})\n",
                     cell1.cell.plmn,
                     cell1.cell.sector_id);
          return false;
        }

        // Two cells on the same frequency should not share the same PRACH root sequence index.
        if ((cell1.cell.prach_cfg.prach_frequency_start == cell2.cell.prach_cfg.prach_frequency_start) &&
            (cell1.cell.prach_cfg.prach_root_sequence_index == cell2.cell.prach_cfg.prach_root_sequence_index)) {
          fmt::print("Warning: cells with PCI {} and {} with the same DL ARFCN (i.e., {}) have the same PRACH root "
                     "sequence index (i.e., {}).\n",
                     cell1.cell.pci,
                     cell2.cell.pci,
                     cell1.cell.dl_arfcn,
                     cell1.cell.prach_cfg.prach_root_sequence_index);
        }
      }
    }
  }

  return true;
}

static bool validate_test_mode_unit_config(const du_high_unit_config& config)
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

template <typename id_type>
static bool validate_rlc_am_appconfig(id_type id, const du_high_unit_rlc_am_config& config)
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

static bool validate_rlc_um_appconfig(five_qi_t five_qi, const du_high_unit_rlc_um_config& config)
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

static bool validate_rlc_unit_config(five_qi_t five_qi, const du_high_unit_rlc_config& config)
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
static bool validate_qos_config(span<const du_high_unit_qos_config> config)
{
  for (const auto& qos : config) {
    if (!validate_rlc_unit_config(qos.five_qi, qos.rlc)) {
      return false;
    }
  }
  return true;
}

bool srsran::validate_du_high_config(const du_high_unit_config& config, const os_sched_affinity_bitmask& available_cpus)
{
  if (!validate_cells_unit_config(config.cells_cfg, config.gnb_id)) {
    return false;
  }

  if (!validate_srb_unit_config(config.srb_cfg)) {
    return false;
  }

  if (!validate_test_mode_unit_config(config)) {
    return false;
  }

  if (!validate_expert_execution_unit_config(config, available_cpus)) {
    return false;
  }

  if (!validate_qos_config(config.qos_cfg)) {
    return false;
  }

  if (config.ntn_cfg.has_value()) {
    if (!validate_ntn_config(config.ntn_cfg.value())) {
      return false;
    }
  }

  if (!validate_pcap_configs(config)) {
    return false;
  }

  return true;
}
